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

#define BOOST_TEST_MAIN
#define BOOST_REQUIRE_MODULE trihlavTestKeyListPresenter
#include <boost/test/included/unit_test.hpp>

#include <FakeIt/single_header/boost/fakeit.hpp>

#include "trihlavGlobalFixture.hpp"
#include "trihlavLib/trihlavIFactory.hpp"
#include "trihlavLib/trihlavIKeyListView.hpp"
#include "trihlavLib/trihlavKeyListPresenter.hpp"

using namespace std;
using namespace trihlav;
using namespace boost;
using namespace boost::filesystem;
using namespace fakeit;

Mock<IFactory> theIFactoryMock;
Mock<IKeyListView> theIKeyListViewMock;

class InitTestKeyListPresenterMocks: virtual public GlobalFixture {
public:
	InitTestKeyListPresenterMocks() :
			GlobalFixture() {
		BOOST_LOG_NAMED_SCOPE("InitTestKeyListPresenterMocks");
		When(Method( theIFactoryMock, createKeyListView)) //< mock factory methods
		.AlwaysReturn(&theIKeyListViewMock.get());
	}
};

BOOST_GLOBAL_FIXTURE(InitTestKeyListPresenterMocks);

BOOST_AUTO_TEST_SUITE(trihlavTestKeyListPresenter)

BOOST_AUTO_TEST_CASE(canAddYubikoKey) {
	BOOST_LOG_NAMED_SCOPE("canAddYubikoKey");
	KeyListPresenter myKeyListPresenter(theIFactoryMock.get());
}

BOOST_AUTO_TEST_SUITE_END()

