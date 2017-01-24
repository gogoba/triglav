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

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>

#include <boost/filesystem.hpp>

#include "gtest/gtest.h"
#include "gmock/gmock.h"  // Brings in Google Mock.

#include "trihlavLib/trihlavLog.hpp"
#include "trihlavLib/trihlavOsIface.hpp"
#include "trihlavLib/trihlavSettings.hpp"
#include "trihlavLib/trihlavLoginPresenter.hpp"

#include "trihlavMockLoginView.hpp"
#include "trihlavMockFactory.hpp"


using ::testing::Return;
using ::testing::NiceMock;
using ::trihlav::initLog;
using ::trihlav::OsIface;
using ::trihlav::Settings;
using ::trihlav::MockFactory;
using ::trihlav::LoginPresenter;
using ::std::string;

static const string K_TST_USR1("test_user1");

class OsIfaceT : virtual public OsIface {
public:
	virtual bool checkOsPswd(const std::string& p_strUName,
			const std::string& p_strPswd) const {
		if(p_strUName=="test1" && p_strPswd=="paswd1") {
			return true;
		}
		return false;
	}
};

class MockFactoryT : virtual public MockFactory {
public:
	OsIfaceT itsOsIface;
	virtual OsIface& getOsIface() {
		return itsOsIface;
	}
};

class TestLogin: public ::testing::Test {
public:
	virtual void SetUp() {
		BOOST_LOG_NAMED_SCOPE("TestLogin::SetUp");
	}

	// Tears down the test fixture.
	virtual void TearDown() {
		BOOST_LOG_NAMED_SCOPE("TestLogin::TearDown");
	}
};

TEST_F(TestLogin,validateUser) {
	BOOST_LOG_NAMED_SCOPE("TestLogin::validateUser");
	MockFactoryT myFactory;
	EXPECT_CALL(myFactory,createLoginView());
	LoginPresenter myLoginPresenter(myFactory);
	myLoginPresenter.getView().getEdtUserName().setValue(::K_TST_USR1);
	myLoginPresenter.getView().getEdtPassword().setValue("paswd1");
	myLoginPresenter.getView().getBtnOk().pressed();
	EXPECT_EQ(myLoginPresenter.getLoggedInUser(),::K_TST_USR1)
		<< "Logged in system user was not passed from view to presenter.";
}


int main(int argc, char **argv) {
	initLog();
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
