/*
 * trihlavWtSysUserListIface.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: grobap
 */

#include <boost/locale.hpp>

#include <Wt/WDialog>
#include <Wt/WTableView>
#include <Wt/WFitLayout>
#include <Wt/WGridLayout>
#include <Wt/WScrollArea>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>

#include "trihlavWtListModel.hpp"

#include "trihlavWtPushButton.hpp"
#include "trihlavWtSysUserListIView.hpp"

namespace {
	static const Wt::WLength K_DLG_H(40.0, Wt::WLength::Unit::FontEm);
	static const Wt::WLength K_DLG_W(30.0, Wt::WLength::Unit::FontEm);
}

namespace trihlav {

using Wt::WTableView;
using Wt::WDialog;
using Wt::WLength;
using Wt::WGridLayout;
using Wt::WFitLayout;
using Wt::WScrollArea;

using Wt::WHBoxLayout;
using Wt::WVBoxLayout;

using boost::locale::translate;
using U = Wt::WLength::Unit;

/**
 * Holds the operating system user list data.
 */
class WtSysUserListModel: public WtListModel<std::string, std::string> {
public:
	WtSysUserListModel() :
		WtListModel< std::string, std::string >( {
					{ translate("System login") }, { translate("Full name") } }) {
	}
};

WtSysUserListView::WtSysUserListView() :
		itsDlg(new WDialog), //
		itsDtaMdl(new WtSysUserListModel), //
		itsSysUserTable(new WTableView), //
		itsCancelBtn(new WtPushButton(translate("Cancel"))), //
		itsOkBtn(new WtPushButton(translate("ok"))) //

{
	BOOST_LOG_NAMED_SCOPE("WtSysUserListView::WtSysUserListView");
	itsDlg->setCaption(translate("Add key").str());
	itsDlg->setObjectName("WtSysUserListView");
	WVBoxLayout* myContentLayout = new WVBoxLayout;
	{
		itsSysUserTable->setObjectName("SysUserTable");

		itsSysUserTable->setModel(itsDtaMdl);
		itsSysUserTable->setAlternatingRowColors(true);
		itsSysUserTable->setCanReceiveFocus(true);
		itsSysUserTable->setColumnResizeEnabled(true);
		itsSysUserTable->setSelectionMode(Wt::SingleSelection);
		myContentLayout->addWidget(itsSysUserTable);
	}
	WHBoxLayout* myBtnLayout = new WHBoxLayout;
	{
		itsCancelBtn->resize(WLength(11.0, U::FontEm), WLength(4.0, U::FontEm));
		itsOkBtn->resize(WLength(11.0, U::FontEm), WLength(4.0, U::FontEm));
		myBtnLayout->addWidget(itsOkBtn);
		myBtnLayout->addWidget(itsCancelBtn);
	}
	itsDlg->contents()->setLayout(myContentLayout);
	itsDlg->contents()->setOverflow(Wt::WContainerWidget::OverflowHidden);
	itsDlg->footer()->setLayout(myBtnLayout);
	itsDlg->setResizable(true);
	itsDlg->finished().connect(this, &WtSysUserListView::finishedSlot);
	itsCancelBtn->clicked().connect(itsDlg.get(), &WDialog::reject);
	itsOkBtn->clicked().connect(itsDlg.get(), &WDialog::accept);
}

void WtSysUserListView::show(const SysUsers& pUsers) {
	BOOST_LOG_NAMED_SCOPE("WtSysUserListView::show");
	int myCnt = 0;
	itsDtaMdl->clear();
	for (const SysUser& myUser : pUsers) {
		myCnt++;
		BOOST_LOG_TRIVIAL(debug)<<"User: " << myUser.itsLogin;
		itsDtaMdl->addRow(WtSysUserListModel::Row_t(std::string(myUser.itsLogin),
				std::string(myUser.itsFullName)));
	}
	BOOST_LOG_TRIVIAL(debug)<<"System users loaded.";
	itsDlg->setModal(true);
	itsDlg->show();
	itsDlg->resize(K_DLG_W, K_DLG_H);
}

void WtSysUserListView::finishedSlot(WDialog::DialogCode pCode) {
	acceptedSig(pCode == WDialog::DialogCode::Accepted);
}

}
/* namespace trihlav */
