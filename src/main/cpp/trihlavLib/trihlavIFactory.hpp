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

#ifndef TRIHLAV_I_FACTORY_HPP_
#define TRIHLAV_I_FACTORY_HPP_

namespace trihlav {

class IMainPanelView;
class IKeyListPresenter;
class IKeyListView;
class IPswdChckPresenter;
class IPswdChckView;
class IYubikoOtpKeyView;
class YubikoOtpKeyPresenterI;

class KeyManager;

class IFactory {
public:
	virtual ~IFactory() {
	}
	virtual IMainPanelView* createMainPanelView()=0;
	virtual IKeyListPresenter* createKeyListPresenter();
	virtual IKeyListView* createKeyListView()=0;
	virtual IPswdChckPresenter* createPswdChckPresenter()=0;
	virtual IPswdChckView* createPswdChckView() =0;
	virtual IYubikoOtpKeyView* createYubikoOtpKeyView() =0;
	virtual KeyManager& getKeyManager() const;
};

}  // namespace trihlav

#endif /* TRIHLAV_I_FACTORY_HPP_ */
