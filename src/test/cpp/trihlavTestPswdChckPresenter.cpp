/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 der GNU General Public License, wie von der Free Software Foundation,
 Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
 veröffentlichten Version, weiterverbreiten und/oder modifizieren.

 Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
 OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 Siehe die GNU General Public License für weitere Details.

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <yubikey.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>

#include <boost/filesystem.hpp>

#include "trihlavLib/trihlavButtonIface.hpp"
#include "trihlavLib/trihlavFactoryIface.hpp"
#include "trihlavLib/trihlavPswdChckPresenter.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"  // Brings in Google Mock.

#include "trihlavLib/trihlavLog.hpp"
#include "trihlavLib/trihlavKeyListPresenter.hpp"
#include "trihlavLib/trihlavKeyManager.hpp"
#include "trihlavLib/trihlavYubikoOtpKeyConfig.hpp"
#include "trihlavLib/trihlavPswdChckViewIface.hpp"
#include "trihlavLib/trihlavPswdChckPresenter.hpp"

#include "trihlavMockButton.hpp"
#include "trihlavMockStrEdit.hpp"
#include "trihlavMockSpinBox.hpp"
#include "trihlavMockFactory.hpp"
#include "trihlavMockEditIface.hpp"
#include "trihlavMockMessageView.hpp"

using namespace std;
using namespace trihlav;
using namespace boost;
using namespace boost::filesystem;

using ::testing::Return;
using ::testing::NiceMock;

class TestPswdChckPresenter: public ::testing::Test {
public:
	virtual void SetUp() {
		BOOST_LOG_NAMED_SCOPE("TestPswdChckPresenter::SetUp");
	}

	// Tears down the test fixture.
	virtual void TearDown() {
		BOOST_LOG_NAMED_SCOPE("TestPswdChckPresenter::TearDown");
	}
};

static const char* K_TST_DESC0 = "Test key 1";
static const char* K_TST_PRIV0="aabbaabbaabb";
static const char* K_TST_PUBL0="ccddccddccdd";
static const char* K_TST_SECU0="ddeeddeeddeeddeeddeeddeeddeeddee";
static const int K_TST_CNTR0 = 1;
static const int K_TST_RNDM0 = 11;

TEST_F(TestPswdChckPresenter,checkPassword) {
	BOOST_LOG_NAMED_SCOPE("TestPswdChckPresenter::checkPassword");
	path myTestCfgFile(unique_path("/tmp/trihlav-tests-%%%%-%%%%"));
	EXPECT_FALSE(exists(myTestCfgFile));
	EXPECT_TRUE(create_directory(myTestCfgFile));
	BOOST_LOG_TRIVIAL(debug)<< "Test data location: " << myTestCfgFile <<".";
	NiceMock<MockFactory> myMockFactory;
	KeyManager& myKeyMan(myMockFactory.getKeyManager());
	myKeyMan.setConfigDir(myTestCfgFile);
	YubikoOtpKeyConfig myCfg0(myKeyMan);
	myCfg0.setDescription(K_TST_DESC0);
	myCfg0.setPrivateId(K_TST_PRIV0);
	myCfg0.setPublicId(K_TST_PUBL0);
	myCfg0.setCounter(K_TST_CNTR0);
	myCfg0.setRandom(K_TST_RNDM0);
	myCfg0.setSecretKey(K_TST_SECU0);
	myCfg0.save();

	myCfg0.getToken().crc = 0;
	myCfg0.getToken().crc = ~ yubikey_crc16(reinterpret_cast<uint8_t*>(&myCfg0.getToken()),
			sizeof(myCfg0.getToken()) - sizeof(myCfg0.getToken().crc));
	char myOtp0[YUBIKEY_OTP_SIZE + 1];
	yubikey_generate(&myCfg0.getToken(), myCfg0.getSecretKeyArray().data(), myOtp0);
	BOOST_LOG_TRIVIAL(debug)<< "Pub ID (M)   : " << myCfg0.getPublicIdModhex() ;
	BOOST_LOG_TRIVIAL(debug)<< "Pub ID (H)   : " << myCfg0.getPublicId();
	BOOST_LOG_TRIVIAL(debug)<< "Generated key: " << myOtp0;
	PswdChckPresenter myPresenter{myMockFactory};
	MockMessageView& myMockMessageView= dynamic_cast<MockMessageView&>
		(myPresenter.getMessageView());
	EXPECT_CALL(myMockMessageView,showMessage("Password check","Password OK!"));
	myPresenter.getView().getEdtPswd0().setValue(myOtp0);
	myPresenter.getView().getBtnOk().getPressedSignal()();
	remove_all(myTestCfgFile);
	EXPECT_FALSE(exists(myTestCfgFile));
}


int main(int argc, char **argv) {
	initLog();
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
