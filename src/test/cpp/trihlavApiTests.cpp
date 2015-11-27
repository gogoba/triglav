#include <yubikey.h>
#include <string>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>

#include <boost/filesystem.hpp>

#define BOOST_TEST_MAIN
#define BOOST_REQUIRE_MODULE trihlavApiTests
#include <boost/test/included/unit_test.hpp>

#include <FakeIt/single_header/boost/fakeit.hpp>

#include "trihlavLib/trihlavYubikoOptKeyPresenter.hpp"
#include "trihlavLib/trihlavUTimestamp.hpp"
#include "trihlavLib/trihlavYubikoOtpKeyConfig.hpp"
#include "trihlavLib/trihlavKeyManager.hpp"
#include "trihlavLib/trihlavYubikoOtpKeyConfig.hpp"
#include "trihlavLib/trihlavIEdit.hpp"
#include "trihlavLib/trihlavIYubikoOptKeyView.hpp"

using namespace std;
using namespace trihlav;
using namespace boost;
using namespace boost::filesystem;
using namespace fakeit;

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;

struct TrihlavApiTestsGlobalFixture {
	/**
	 * Tear up. Called beforeall. Sets up logging.
	 */
	TrihlavApiTestsGlobalFixture() {
		BOOST_LOG_NAMED_SCOPE("TrihlavApiTestsGlobalFixture");
		boost::log::add_common_attributes();
		boost::shared_ptr<boost::log::core> p_core = boost::log::core::get();
		p_core->add_global_attribute("Scope", attrs::named_scope());
		/* log formatter:
		 * [TimeStamp] [ThreadId] [Severity Level] [Scope] Log message
		 */
		auto fmtTimeStamp = boost::log::expressions::format_date_time<
				boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
		auto fmtThreadId = boost::log::expressions::attr<
				boost::log::attributes::current_thread_id::value_type>(
				"ThreadID");
		auto fmtSeverity = boost::log::expressions::attr<
				boost::log::trivial::severity_level>("Severity");
		auto fmtScope = boost::log::expressions::format_named_scope("Scope",
				boost::log::keywords::format = "%n",
				boost::log::keywords::iteration =
						boost::log::expressions::reverse,
				boost::log::keywords::depth = 2);
		boost::log::formatter logFmt = boost::log::expressions::format(
				"[%1%] (%2%) [%3% \t] [%4%] %5%") % fmtTimeStamp % fmtThreadId
				% fmtSeverity % fmtScope % boost::log::expressions::smessage;

		/* console sink */
		auto consoleSink = boost::log::add_console_log(std::clog);
		consoleSink->set_formatter(logFmt);
		BOOST_LOG_TRIVIAL(info)<< "global setup ok";
	}

	/**
	 * Called on tear down.
	 */
	~TrihlavApiTestsGlobalFixture() {
		BOOST_LOG_NAMED_SCOPE("~TrihlavApiTestsGlobalFixture");
		BOOST_LOG_TRIVIAL(info) << "global teardown\n";
	}
};

BOOST_GLOBAL_FIXTURE(TrihlavApiTestsGlobalFixture)

inline void logDebug_token(const yubikey_token_st& pToken) {
	BOOST_LOG_NAMED_SCOPE("logDebug_token");
	std::string myUid(YUBIKEY_UID_SIZE * 2 + 1, ' ');
	yubikey_hex_encode(&myUid[0], reinterpret_cast<const char*>(&pToken.uid),
	YUBIKEY_UID_SIZE);
	BOOST_LOG_TRIVIAL(debug)<< "yubikey_token_st:{";
	BOOST_LOG_TRIVIAL(debug)<< "   uid  :\""<< myUid.c_str() << "\"";
	BOOST_LOG_TRIVIAL(debug)<< "   ctr  :\""<< int(pToken.ctr) << "\"";
	BOOST_LOG_TRIVIAL(debug)<< "   use  :\""<< int(pToken.use) << "\"";
	BOOST_LOG_TRIVIAL(debug)<< "   rnd  :\""<< int(pToken.rnd) << "\"";
	BOOST_LOG_TRIVIAL(debug)<< "   tstpl:\""<< int(pToken.tstpl)<< "\"";
	BOOST_LOG_TRIVIAL(debug)<< "   tstph:\""<< int(pToken.tstph)<< "\"";
	BOOST_LOG_TRIVIAL(debug)<< "   crc  :\""<< int(pToken.crc) << "\"";
	BOOST_LOG_TRIVIAL(debug)<< "}";
}

BOOST_AUTO_TEST_SUITE(trihlavApiTestsSuit)

/**
 * Test the yubico-c library text to string calls.
 */
BOOST_AUTO_TEST_CASE( testHexToString ) {
	BOOST_LOG_NAMED_SCOPE("testHexToString");
	const char* K_TEST0 = "quantum";
	const size_t K_TEST0_SIZE = strlen(K_TEST0);
	std::string myTest0HexEncoded(K_TEST0_SIZE * 2 + 1, ' ');
	const std::string K_EXPECTED("7175616e74756d\0", K_TEST0_SIZE * 2 + 1);
	yubikey_hex_encode(&myTest0HexEncoded[0], K_TEST0, K_TEST0_SIZE);
	BOOST_LOG_TRIVIAL(debug)<< "Encoded \""<<K_TEST0<<"\" is \""
	<<myTest0HexEncoded.c_str() <<"\"";
	BOOST_REQUIRE(myTest0HexEncoded == K_EXPECTED);
	BOOST_LOG_TRIVIAL(debug)<< "testHexToString() ok";
}

/**
 * Generate two OTPs, parse them and check token parts
 * @return void
 */
BOOST_AUTO_TEST_CASE(testGenerateAndParse) {
	BOOST_LOG_NAMED_SCOPE("testGenerateAndParse");
	/* Decrypt TOKEN using KEY and store output in OUT structure.  Note
	 that there is no error checking whether the output data is valid or
	 not, use yubikey_check_* for that. */
	const uint8_t myKey[YUBIKEY_KEY_SIZE] = { 0xaa, 0xff, 0x0c, 0x00, 0xa1,
			0xef, 0x1c, 0x10, 0xa2, 0xdf, 0x3c, 0x30, 0xa3, 0xff, 0xcc, 0x60 };
	const char K_Y_UID[YUBIKEY_UID_SIZE + 1] = "012543";
	/* Generate OTP */
	yubikey_token_st myToken, myTokenBack;
	const size_t K_YBK_TKEN_SZ = sizeof(myToken);
	BOOST_LOG_TRIVIAL(debug)<< "sizeof(time_t)==%d",int(sizeof(time_t));
	UTimestamp myTstp;
	myTstp.tstp_int = time(0);
	memcpy(&myToken.uid, K_Y_UID, YUBIKEY_UID_SIZE);
	myToken.ctr = 3;
	myToken.use = 4;
	myToken.rnd = 42;
	myToken.tstpl = myTstp.tstp.tstpl;
	myToken.tstph = myTstp.tstp.tstph;
	myToken.crc = 0;
	myToken.crc = ~yubikey_crc16(reinterpret_cast<uint8_t*>(&myToken),
			K_YBK_TKEN_SZ - sizeof(myToken.crc));
	memcpy(&myTokenBack, &myToken, sizeof(myToken));
	logDebug_token(myTokenBack);
	char myOtp0[YUBIKEY_OTP_SIZE], myOtp1[YUBIKEY_OTP_SIZE];
	yubikey_generate(&myToken, myKey, myOtp0);
	BOOST_LOG_TRIVIAL(debug)<< "Generated yubikey OTP (0) " << myOtp0;
	yubikey_parse(reinterpret_cast<uint8_t*>(myOtp0), myKey, &myToken);
	logDebug_token(myToken);
	BOOST_REQUIRE(myTokenBack.ctr == myToken.ctr);
	BOOST_REQUIRE(myTokenBack.rnd == myToken.rnd);
	BOOST_REQUIRE(myTokenBack.use == myToken.use);
	BOOST_REQUIRE(myTokenBack.tstph == myToken.tstph);
	BOOST_REQUIRE(myTokenBack.tstpl == myToken.tstpl);
	BOOST_REQUIRE_MESSAGE(
			strncmp( reinterpret_cast<const char*>(&myTokenBack.uid), reinterpret_cast<char*>(&myToken.uid), YUBIKEY_UID_SIZE)==0,
			"The uid-s are different!");
	uint16_t myCrc = yubikey_crc16(reinterpret_cast<uint8_t*>(&myToken),
	YUBIKEY_KEY_SIZE);
	BOOST_LOG_TRIVIAL(debug)<< "crc1="<<myCrc <<" - "<<YUBIKEY_CRC_OK_RESIDUE;
	BOOST_REQUIRE_MESSAGE(
	yubikey_crc_ok_p(reinterpret_cast<uint8_t*>(&myToken)), "CRC failed!");
	yubikey_generate(&myToken, myKey, myOtp1);
	BOOST_LOG_TRIVIAL(debug)<< "Generated yubikey OTP (1) "<<myOtp1;
	BOOST_REQUIRE_MESSAGE(strncmp(myOtp0,myOtp1,YUBIKEY_OTP_SIZE)==0,
			"The (re)generatet keys are different!");
	BOOST_LOG_TRIVIAL(debug)<< "testGenerateAndParse() ok";
}

BOOST_AUTO_TEST_CASE(testUTimestampMemLayout) {
	BOOST_LOG_NAMED_SCOPE("testUTimestampMemLayout");
	UTimestamp myTstp;
	myTstp.tstp_int = 0x6789ABCD;
	BOOST_REQUIRE_MESSAGE(sizeof(myTstp) == 4,
			"Memory alignment is not bytewise!");
	BOOST_REQUIRE(int(myTstp.tstp.tstpl) == int(0x6789));
	BOOST_REQUIRE(int(myTstp.tstp.tstph) == int(0xAB));
	BOOST_REQUIRE(int(myTstp.tstp.filler) == int(0xCD));
}

BOOST_AUTO_TEST_CASE(testLoadAndSaveKeyCfg) {
	BOOST_LOG_NAMED_SCOPE("testLoadAndSaveKeyCfg");
	path myTestCfgFile(unique_path("%%%%-%%%%-%%%%-%%%%.json"));
	BOOST_LOG_TRIVIAL(debug)<< "Test data location: '" << myTestCfgFile <<"'.";
	YubikoOtpKeyConfig myTestCfg0(myTestCfgFile);
	myTestCfg0.setPrivateId("010203040506");
	myTestCfg0.setTimestamp(UTimestamp(12345));
	myTestCfg0.setCounter(33);
	myTestCfg0.setRandom(44);
	myTestCfg0.setCrc(55);
	myTestCfg0.setUseCounter(7);
	BOOST_REQUIRE(int(myTestCfg0.getToken().uid[0]) == 1);
	BOOST_REQUIRE(int(myTestCfg0.getToken().uid[1]) == 2);
	BOOST_REQUIRE(int(myTestCfg0.getToken().uid[2]) == 3);
	BOOST_REQUIRE(int(myTestCfg0.getToken().uid[3]) == 4);
	BOOST_REQUIRE(int(myTestCfg0.getToken().uid[4]) == 5);
	BOOST_REQUIRE(int(myTestCfg0.getToken().uid[5]) == 6);
	BOOST_LOG_TRIVIAL(debug)<< "Private id:'" << myTestCfg0.getPrivateId() << "'.";
	BOOST_REQUIRE(myTestCfg0.getPrivateId().compare("010203040506") == 0);

	BOOST_REQUIRE(int(myTestCfg0.getToken().ctr) == 33);
	BOOST_REQUIRE(int(myTestCfg0.getToken().rnd) == 44);
	BOOST_REQUIRE(int(myTestCfg0.getToken().crc) == 55);
	BOOST_REQUIRE(int(myTestCfg0.getToken().use) == 7);

	myTestCfg0.save();
	YubikoOtpKeyConfig myTestCfg1(myTestCfgFile);
	myTestCfg1.load();
	BOOST_REQUIRE(myTestCfg0.getPrivateId() == myTestCfg1.getPrivateId());
	BOOST_REQUIRE(
			myTestCfg0.getTimestamp().tstp_int
					== myTestCfg1.getTimestamp().tstp_int);
	BOOST_REQUIRE(myTestCfg0.getCounter() == myTestCfg1.getCounter());
	BOOST_REQUIRE(myTestCfg0.getCrc() == myTestCfg1.getCrc());
	BOOST_REQUIRE(myTestCfg0.getRandom() == myTestCfg1.getRandom());
	BOOST_REQUIRE(myTestCfg0.getUseCounter() == myTestCfg1.getUseCounter());

	remove(myTestCfgFile);
	BOOST_LOG_TRIVIAL(debug)<< "test file removed, testLoadAndSaveKeyCfg ok";
}

string aPrivateIdStr;
string aPublicIdStr;
int aPublicIdLen=666;

BOOST_AUTO_TEST_CASE(testKeyManager) {
	BOOST_LOG_NAMED_SCOPE("testKeyManager");
	KeyManager myKMan(unique_path("/tmp/trihlav-%%%%-%%%%-%%%%-%%%%"));
	BOOST_LOG_TRIVIAL(debug)<< "Test lazy init. only first getter will cause"
	" initalization";
	BOOST_REQUIRE(!myKMan.isInitialized());
	const path& myKManPath = myKMan.getConfigDir();
	BOOST_LOG_TRIVIAL(debug)<< "Got config directory \"" << myKManPath << "\","
	" now we should be initialized";

	BOOST_REQUIRE(myKMan.isInitialized());
	BOOST_REQUIRE(exists(myKManPath));

	Mock<IStrEdit> myPrivateIdStrEditMock;
	Mock<IStrEdit> myPublicIdStrEditMock;
	Mock<IEdit<int>> myPublicIdLenEditMock;

	When(Method(myPrivateIdStrEditMock,setValue)).AlwaysDo([](const string& pVal) {
		aPrivateIdStr=pVal;
		BOOST_LOG_TRIVIAL(debug)<< "Priv id == \"" << aPrivateIdStr << "\".";
	});

	Mock<IYubikoOptKeyView> myMockYubikoOptKeyView;
	When(ConstOverloadedMethod( myMockYubikoOptKeyView, getPublicIdentity, const IStrEdit& () )).AlwaysReturn(
			myPublicIdStrEditMock.get());
	When(OverloadedMethod( myMockYubikoOptKeyView, getPublicIdentity, IStrEdit& () )).AlwaysReturn(
			myPublicIdStrEditMock.get());
	When(ConstOverloadedMethod( myMockYubikoOptKeyView, getPrivateIdentity, const IStrEdit& () )).AlwaysReturn(
			myPrivateIdStrEditMock.get());
	When(OverloadedMethod( myMockYubikoOptKeyView, getPrivateIdentity, IStrEdit& () )).AlwaysReturn(
			myPrivateIdStrEditMock.get());
	When(ConstOverloadedMethod( myMockYubikoOptKeyView, getPublicIdentityLen, const IEdit<int>& () )).AlwaysReturn(
			myPublicIdLenEditMock.get());
	When(OverloadedMethod( myMockYubikoOptKeyView, getPublicIdentityLen, IEdit<int>& () )).AlwaysReturn(
			myPublicIdLenEditMock.get());

	YubikoOptKeyPresenter myPresenter(myMockYubikoOptKeyView.get());
	Verify(Method(myPrivateIdStrEditMock,setValue)/*.Using("")*/).AtLeastOnce();
	BOOST_REQUIRE(aPrivateIdStr.empty());
	BOOST_REQUIRE(remove_all(myKManPath));
	BOOST_LOG_TRIVIAL(debug)<< "testKeyManager ok";
}

BOOST_AUTO_TEST_SUITE_END()
