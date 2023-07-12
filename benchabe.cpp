#include <iostream>
#include "cxxopts.hpp"
#include "abecontext.h"

using namespace lbcrypto;
using namespace std;

usint ringsize = 1024;
usint numAttributes = 6;
usint base = 64;
usint iterations = 100;

bool verbose = false;
bool twoPhase = false;

bool parse(int argc, char* argv[]){

	try{
		cxxopts::Options options(argv[0], "- program for benchmark cp-abe scheme performance under specified parameters");
	
		options.set_width(70).allow_unrecognised_options().add_options()
			("r,ringsize", "set ringsize value", cxxopts::value<usint>()->default_value(to_string(ringsize)))
			("a,attributes", "set the number of attributes", cxxopts::value<usint>()->default_value(to_string(numAttributes)))
			("b,base", "set lattice's base value", cxxopts::value<usint>()->default_value(to_string(base)))
			("o,offline", "set scheme with offline and online phase", cxxopts::value<bool>()->default_value("false"))
			("i,iterations", "set the number of iterations", cxxopts::value<usint>()->default_value(to_string(iterations)))
			("v,verbose", "enable verbose output", cxxopts::value<bool>()->default_value("false"))
			("h,help", "print help");
	
		auto result = options.parse(argc, argv);
	
	
		if (result.count("help")){	
			cout << options.help() << endl;
			return false;		
		}
	
		if (result.count("ringsize")){
			ringsize = result["ringsize"].as<usint>();
		}
	
		if (result.count("attributes")){
			numAttributes = result["attributes"].as<usint>();
		}
	
		if (result.count("base")){
			base = result["base"].as<usint>();
		}
	
		if (result.count("iterations")){
			iterations = result["iterations"].as<usint>();
		}
	
		twoPhase = result["offline"].as<bool>();
		verbose = result["verbose"].as<bool>();
		
		/*
		cout << "Unmatched options: ";
	    	for (const auto& option: result.unmatched()){
	      		cout << "'" << option << "' ";
	    	}
	    	cout << "ignored." << endl;
	    	*/
	}
	catch (const cxxopts::exceptions::exception& e){
		cerr << "error parsing options: " << e.what() << endl;
		return false;
	}

	return true;
}

int main(int argc, char* argv[]){

	TimeVar t1;
	float duration;
	float s = 0;

	CPABEMasterPublicKey<NativePoly> mpk;
	CPABEMasterSecretKey<NativePoly> msk;
	CPABECiphertext<NativePoly> ct;
	Plaintext dt;

	cout << fixed;
  	cout.precision(3);

	// clean terminal
	cout << u8"\033[2J\033[1;1H";

	// parse command line arguments
	if (!parse(argc, argv)){ return 0; }

	// print chosen parameters
  	cout << "This is a demo file of the CP-ABE scheme" << endl << endl;
  	cout << "Used parameters:" << endl;
  	cout << "Ring size: " << ringsize << endl;
  	cout << "Number of attributes: " << numAttributes << endl;
  	cout << "Base: " << base << endl;
  	string res = (twoPhase) ? "Offline / Online phase" : "Single phase";
  	cout << "Keygen: " << res << endl;
  	cout << "Iterations: " << iterations << endl << endl;

  	// create context
  	ABEContext<NativePoly> context;
  	cout << "Generating a context under these parameters..." << endl << endl;
  	context.GenerateCPABEContext(numAttributes, ringsize, base);


	// setup scheme
	cout << "Generating master secret key and master public key..." << endl;

	for (usint i = 1; i <= iterations; ++i){
		
		TIC(t1);
		context.Setup(&mpk, &msk);
		duration = TOC(t1);

		if (verbose){ cout << "Setup (" << i << "): " << duration << " ms" << endl; }
		
		s = s + duration;
	}
	cout << "Setup average duration: " << s/iterations << " ms" << endl << endl;


	// Create a random access policy and user attribute set
  	cout << "Creating access policy and user attribute sets..." << endl;
  	vector<usint> userAttrSet(numAttributes);
  	vector<int> accPolicy(numAttributes);

  	for (usint j = 0; j < numAttributes; j++) userAttrSet[j] = rand() % 2;

  	for (usint j = 0; j < numAttributes; j++) accPolicy[j] = userAttrSet[j];

  	for (usint j = 0; j < numAttributes; j++)
    	if (accPolicy[j] == 1) {
      		accPolicy[j] = 0;
      		break;
    	}

  	for (usint j = 0; j < numAttributes; j++)
    	if (userAttrSet[j] == 0) {
      		accPolicy[j] = -1;
      		break;
    	}

  	cout << "User attribute set: " << userAttrSet << endl;
  	cout << "Access policy defined: " << accPolicy << endl << endl;
  	CPABEUserAccess<NativePoly> ua(userAttrSet);
  	CPABEAccessPolicy<NativePoly> ap(accPolicy);


 	// Create the key corresponding to the access policy
	CPABESecretKey<NativePoly> sk;
  	cout << "Creating secret key for the attribute set..." << endl;

  	s = 0;
  	if (!twoPhase){

  		// single phase
		for (usint i = 1; i <= iterations; ++i){
		
			TIC(t1);
	  		context.KeyGen(msk, mpk, ua, &sk);
	  		duration = TOC(t1);

	  		if (verbose){ cout << "KeyGen (" << i << "): " << duration << " ms" << endl; }

			s = s + duration;

		}

		cout << "KeyGen average duration: " << s/iterations << " ms" << endl << endl;
	
	}else{

		// two phases
		// offline phase
		PerturbationVector<NativePoly> pv;

		for(usint i = 1; i <= iterations; ++i){

			TIC(t1);
  			context.KeyGenOfflinePhase(msk, pv);
  			duration = TOC(t1);

  			if(verbose){ cout << "KeyGen Offline (" << i << "): " << duration << " ms" << endl; }

  			s = s + duration;
		}

		cout << "KeyGen Offline average duration: " << s/iterations << " ms" << endl;


		// online phase
		s = 0;

		for(usint i = 1; i <= iterations; ++i){

			TIC(t1);
  			context.KeyGenOnlinePhase(msk, mpk, ua, pv, &sk);
  			duration = TOC(t1);

  			if(verbose){ cout << "KeyGen Online (" << i << "): " << duration << " ms" << endl; }

  			s = s + duration;
		}

		cout << "KeyGen Online average duration: " << s/iterations << " ms" << endl << endl;

	}

  	// Create a plaintext
  	cout << "Generating test plaintext..." << endl;
  	vector<int64_t> vectorOfInts = {1, 0, 0, 1, 1, 0, 1, 0, 1, 0};
  	Plaintext pt = context.MakeCoefPackedPlaintext(vectorOfInts);
  	cout << "Plaintext vector of bits: " << vectorOfInts << endl << endl;


  	// Encrypt the plaintext
  	cout << "Encrypting the plaintext under the access policy..." << endl;
  	
  	s = 0;
  	for (usint i = 1; i <= iterations; ++i){

  		TIC(t1);
  		context.Encrypt(mpk, ap, pt, &ct);
  		duration = TOC(t1);

  		if (verbose){ cout << "Encryption (" << i << "): " << duration << " ms" << endl; }

		s = s + duration;
  	}
  	cout << "Encryption average duration: " << s/iterations << " ms" << endl << endl;


  	// Decrypt the ciphertext
  	cout << "Decrpyting the ciphertext..." << endl;

  	s = 0;
  	for (usint i = 1; i <= iterations; ++i){

  		TIC(t1);
  		dt = context.Decrypt(ap, ua, sk, ct);
  		duration = TOC(t1);
  		
  		if (verbose){ cout << "Decryption (" << i << "): " << duration << " ms" << endl; }
  		
  		s = s + duration;
  	}
  	cout << "Decryption average duration: " << s/iterations << " ms" << endl << endl;


	// Check if original plaintext and decrypted plaintext match
  	cout << "Checking if the plaintext & decrypted text match..." << endl;
  	if (pt->GetElement<NativePoly>() == dt->GetElement<NativePoly>()) {
    	cout << "Encryption & decryption successful" << endl << endl;
  	} else {
    	cout << "Encryption & decryption failed" << endl << endl;
  	}

	return 0;
}
