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

#include <boost/locale.hpp>

#include <Wt/WDialog>
#include <Wt/WPushButton>
#include <Wt/WHBoxLayout>

#include "trihlavLib/trihlavLogApi.hpp"
#include "trihlavWtDialogView.hpp"
#include "trihlavWtPushButton.hpp"

using Wt::WDialog;
using Wt::WLength;
using Wt::WHBoxLayout;
using std::string;
using boost::locale::translate;
using U = Wt::WLength::Unit;

namespace trihlav {
    WtDialogView::WtDialogView() : //
            itsDlg(new WDialog), //
            itsCancelBtn(new WtPushButton(translate("Cancel"))), //
            itsOkBtn(new WtPushButton(translate("ok")))//

    {
        BOOST_LOG_NAMED_SCOPE("WtDialogView::WtDialogView()");
        WHBoxLayout* myBtnLayout = new WHBoxLayout;
        {
            itsCancelBtn->resize(WLength(11.0, U::FontEm), WLength(4.0, U::FontEm));
            itsOkBtn->resize(WLength(11.0, U::FontEm), WLength(4.0, U::FontEm));
            myBtnLayout->addWidget(itsOkBtn);
            myBtnLayout->addWidget(itsCancelBtn);
        }
        itsDlg->footer()->setLayout(myBtnLayout);
        itsDlg->finished().connect(this, &WtDialogView::finishedSlot);
        itsCancelBtn->clicked().connect(itsDlg.get(), &WDialog::reject);
        itsOkBtn->clicked().connect(itsDlg.get(), &WDialog::accept);
    }

    const ButtonIface& WtDialogView::getBtnCancel() const {
        return *itsCancelBtn;
    }

    ButtonIface& WtDialogView::getBtnCancel() {
        return *itsCancelBtn;
    }

    const ButtonIface& WtDialogView::getBtnOk() const {
        return *itsOkBtn;
    }

    ButtonIface& WtDialogView::getBtnOk() {
        return *itsOkBtn;
    }

    void WtDialogView::show() {
        itsDlg->show();
    }

    void WtDialogView::finishedSlot(WDialog::DialogCode pCode) {
        acceptedSig(pCode == WDialog::DialogCode::Accepted);
    }

    Wt::WDialog &WtDialogView::getDlg() {
        return *itsDlg;
    }
}
