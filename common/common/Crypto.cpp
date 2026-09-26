#include "Crypto.h"
#include <wincrypt.h>

#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Crypt32.lib")

namespace crypto {

	std::string to_base64(const unsigned char* data, size_t len) {
		DWORD out_len = 0;
		if (!CryptBinaryToStringA(data, (DWORD)len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &out_len))
			return "";
		std::string out(out_len, '\0');
		if (!CryptBinaryToStringA(data, (DWORD)len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &out[0], &out_len))
			return "";
		out.resize(out_len);
		return out;
	}

	std::vector<unsigned char> from_base64(const std::string& b64) {
		DWORD out_len = 0;
		if (!CryptStringToBinaryA(b64.c_str(), (DWORD)b64.size(), CRYPT_STRING_BASE64, NULL, &out_len, NULL, NULL))
			return {};
		std::vector<unsigned char> out(out_len);
		if (!CryptStringToBinaryA(b64.c_str(), (DWORD)b64.size(), CRYPT_STRING_BASE64, out.data(), &out_len, NULL, NULL))
			return {};
		out.resize(out_len);
		return out;
	}

	RsaKeys::RsaKeys() {
		_alg = NULL;
		_own = NULL;
		_peer = NULL;
	}

	RsaKeys::~RsaKeys() {
		if (_own)
			BCryptDestroyKey(_own);
		if (_peer)
			BCryptDestroyKey(_peer);
		if (_alg)
			BCryptCloseAlgorithmProvider(_alg, 0);
	}

	bool RsaKeys::generate() {
		if (BCryptOpenAlgorithmProvider(&_alg, BCRYPT_RSA_ALGORITHM, NULL, 0) != 0)
			return false;
		if (BCryptGenerateKeyPair(_alg, &_own, 2048, 0) != 0)
			return false;
		if (BCryptFinalizeKeyPair(_own, 0) != 0)
			return false;
		return true;
	}

	std::string RsaKeys::public_key_b64() {
		DWORD len = 0;
		if (BCryptExportKey(_own, NULL, BCRYPT_RSAPUBLIC_BLOB, NULL, 0, &len, 0) != 0)
			return "";
		std::vector<unsigned char> blob(len);
		if (BCryptExportKey(_own, NULL, BCRYPT_RSAPUBLIC_BLOB, blob.data(), len, &len, 0) != 0)
			return "";
		return to_base64(blob.data(), len);
	}

	bool RsaKeys::set_peer_key_b64(const std::string& b64) {
		std::vector<unsigned char> blob = from_base64(b64);
		if (blob.empty())
			return false;

		std::lock_guard<std::mutex> lock(_mtx);
		if (_peer) {
			BCryptDestroyKey(_peer);
			_peer = NULL;
		}
		return BCryptImportKeyPair(_alg, NULL, BCRYPT_RSAPUBLIC_BLOB, &_peer, blob.data(), (ULONG)blob.size(), 0) == 0;
	}

	bool RsaKeys::has_peer_key() {
		std::lock_guard<std::mutex> lock(_mtx);
		return _peer != NULL;
	}

	std::string RsaKeys::encrypt_for_peer(const std::string& text) {
		std::lock_guard<std::mutex> lock(_mtx);
		if (!_peer)
			return "";

		DWORD len = 0;
		if (BCryptEncrypt(_peer, (PUCHAR)text.c_str(), (ULONG)text.size(), NULL, NULL, 0, NULL, 0, &len, BCRYPT_PAD_PKCS1) != 0)
			return "";
		std::vector<unsigned char> out(len);
		if (BCryptEncrypt(_peer, (PUCHAR)text.c_str(), (ULONG)text.size(), NULL, NULL, 0, out.data(), len, &len, BCRYPT_PAD_PKCS1) != 0)
			return "";
		return to_base64(out.data(), len);
	}

	std::string RsaKeys::decrypt(const std::string& b64) {
		std::vector<unsigned char> blob = from_base64(b64);
		if (blob.empty())
			return "";

		DWORD len = 0;
		if (BCryptDecrypt(_own, blob.data(), (ULONG)blob.size(), NULL, NULL, 0, NULL, 0, &len, BCRYPT_PAD_PKCS1) != 0)
			return "";
		std::vector<unsigned char> out(len);
		if (BCryptDecrypt(_own, blob.data(), (ULONG)blob.size(), NULL, NULL, 0, out.data(), len, &len, BCRYPT_PAD_PKCS1) != 0)
			return "";
		return std::string((char*)out.data(), len);
	}
}
