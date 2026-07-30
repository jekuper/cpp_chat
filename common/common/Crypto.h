#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>
#include <mutex>

namespace crypto {

	// rsa 2048 with pkcs1 padding, keys live only for the session
	class RsaKeys {
	public:
		RsaKeys();
		~RsaKeys();

		RsaKeys(const RsaKeys&) = delete;
		RsaKeys& operator=(const RsaKeys&) = delete;

		bool generate();
		std::string public_key_b64();

		bool set_peer_key_b64(const std::string& b64);
		bool has_peer_key();

		std::string encrypt_for_peer(const std::string& text);
		std::string decrypt(const std::string& b64);

	private:
		BCRYPT_ALG_HANDLE _alg;
		BCRYPT_KEY_HANDLE _own;
		BCRYPT_KEY_HANDLE _peer;
		std::mutex _mtx;
	};

	std::string to_base64(const unsigned char* data, size_t len);
	std::vector<unsigned char> from_base64(const std::string& b64);
}
