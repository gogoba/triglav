#ifndef TRIHLAV_YubikoOtpKeyConfig_HPP_
#define TRIHLAV_YubikoOtpKeyConfig_HPP_

#include <yubikey.h>
#include <string>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>

#include "trihlavLib/trihlavUTimestamp.hpp"

namespace bfs = ::boost::filesystem;

namespace trihlav {

class KeyManager;

/**
 * @brief Store, load and provide the APIs configuration.
 */
class YubikoOtpKeyConfig {
public:
	static const size_t K_MAX_SYS_USER_LEN = 1024;

	using SecretKeyArr=std::array<uint8_t,YUBIKEY_KEY_SIZE>;
	/**
	 * @brief YubikoOtpKeyConfig::YubikoOtpKeyConfig
	 */
	YubikoOtpKeyConfig(KeyManager& pKeyManager, const bfs::path& pFilename);

	/**
	 * @brief This constructor will generate the filename,
	 */
	YubikoOtpKeyConfig(KeyManager& pKeyManager);

	/**
	 * @brief getPrivateId access the private id config value
	 */
	const std::string getPrivateId() const;

	/**
	 * @brief Timestamp incremented by approx 8Hz.
	 * @see UTimestamp
	 * @return yubikey_token_st.tstpl and yubikey_token_st.tstph
	 */
	const UTimestamp getTimestamp() const {
		UTimestamp myRetVal;
		myRetVal.tstp.tstpl = itsToken.tstpl;
		myRetVal.tstp.tstph = itsToken.tstph;
		return myRetVal;
	}

	/**
	 * @brief setter, see getTimestamp()
	 * @param pVal new value of timestamp values.
	 */
	void setTimestamp(const UTimestamp pVal) {
		itsToken.tstpl = pVal.tstp.tstpl;
		itsToken.tstph = pVal.tstp.tstph;
	}

	/**
	 * @brief setPrivateId sets the private id part of the Yubikey token
	 */
	void setPrivateId(const std::string& pPrivateId);

	/**
	 * @brief The random value of Yubiko token.
	 * @return the random filler as integer.
	 */
	uint16_t getRandom() const {
		return getToken().rnd;
	}

	/**
	 * @brief @see getRandom()
	 * @param pRandom new random value of the Yubiko token.
	 */
	void setRandom(const uint16_t pRandom) {
		getToken().rnd = pRandom;
	}

	/**
	 * @brief Number of times used within session + activation flags.
	 * @return The use counter in the Yubiko token.
	 */
	uint8_t getCounter() const {
		return getToken().ctr;
	}

	/**
	 * @brief @see getCounter()
	 * @param pCounter The new value for the use counter.
	 */
	void setCounter(const uint16_t pCounter) {
		getToken().ctr = pCounter;
	}

	/**
	 * @brief Number of times used within session + activation flags.
	 * @return The use counter in the Yubiko token.
	 */
	uint8_t getUseCounter() const {
		return getToken().use;
	}

	/**
	 * @brief @see getUse()
	 * @param pUse The new value for the use counter.
	 */
	void setUseCounter(const uint8_t pUse) {
		getToken().use = pUse;
	}

	/**
	 * @brief CRC16 value of all fields.
	 * @return The CRC16 value in the Yubiko token.
	 */
	uint16_t getCrc() const {
		return getToken().crc;
	}

	/**
	 * @brief @see getCrc()
	 * @param pCrc The new value for the CRC16 value.
	 */
	void setCrc(const uint16_t pCrc) {
		getToken().crc = pCrc;
	}

	/**
	 * @brief save the configuration values to passed filename.
	 */
	void save();

	/**
	 * @brief load the configuration values from passed filename.
	 */
	void load();

	/**
	 * @brief ~YubikoOtpKeyConfig
	 */
	virtual ~YubikoOtpKeyConfig();

	/// @brief Getter.
	const bfs::path& getFilename() const {
		return itsFilename;
	}

	void generateFilename();

	/**
	 * @return The Yubikey constant token.
	 */
	const yubikey_token_st& getToken() const {
		return itsToken;
	}

	/**
	 * @return The Yubikey token.
	 */
	yubikey_token_st& getToken() {
		return itsToken;
	}

	const std::string& getPublicId() const {
		return itsPublicId;
	}

	/// @brief Set the public id and update indexes.
	void setPublicId(const std::string& pPubId);

	const std::string getPublicIdModhex() const;

	/**
	 * @see getSecretKey()
	 *
	 * @return the secret key binnary array.
	 */
	const SecretKeyArr& getSecretKeyArray() const {
		return itsKey;
	}

	/**
	 * @see setSecretKey(const std::string& pKey)
	 *
	 * @return the secret key, hex encoded.
	 */
	const std::string getSecretKey() const;

	/**
	 * @brief Set the secret key, 16 bytes 32 characters hex encoded.
	 *
	 * Not interpreted technically.
	 *
	 * @param pKey new value for the secret key, hex encoded.
	 */
	void setSecretKey(const std::string& pKey);

	/**
	 * @brief getter
	 * @return users description of the key.
	 *
	 * Not interpreted technically.
	 *
	 * @see itsDescription
	 */
	const std::string& getDescription() const {
		return itsDescription;
	}

	/**
	 * @brief setter, sets the user description.
	 * @see itsDescription
	 */
	void setDescription(const std::string& pDesc) {
		if (pDesc.compare(getDescription()) != 0) {
			itsDescription = pDesc;
			itsChangedFlag = true;
		}
	}

	/// @brief compare two configurations
	bool operator ==(const YubikoOtpKeyConfig& pOther) const;

	const std::string token2json() const;

	static const std::string hex2Modhex(const std::string& p2Modhex);

	static const std::string modhex2Hex(const std::string& p2Hex);

	/// @brief check a modhex encoded password
	bool checkOtp(const std::string& pPswd2check);

	/**
	 *  @brief Compute CRC, store it in token and return it.
	 *  @return the newly computed CRC.
	 */
	uint16_t computeCrc() {
		setCrc(computeCrc(getToken()));
		return getCrc();
	}

	const std::string& getSysUser() const {
		return itsSysUser;
	}

	void setSysUser(const std::string& pSysUser);

	static uint16_t computeCrc(const yubikey_token_st& pToken);

	const std::string checkFileName(bool pIsOut) const;

protected:
	void setFilename(const std::string &value);
	void zeroToken();
	void copyAndSaveToken(const yubikey_token_st& pToken);

private:
	std::string itsPublicId;    //< Keys public ID max 6 characters.
	bool itsChangedFlag;        //< will be set internal when something changed
	bfs::path itsFilename;      //< where to store it
	yubikey_token_st itsToken;
	SecretKeyArr itsKey;
	std::string itsDescription; //< Users free text describing the key
	KeyManager& itsKeyManager;  //< Global functionality & data
	std::string itsSysUser;     //< assotiated system user
};

} // end namespace trihlavApi

#endif // TRIHLAV_YubikoOtpKeyConfig_HPP_
