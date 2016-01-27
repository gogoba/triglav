/*
 * trihlavIKeyListView.hpp
 *
 *  Created on: 18.01.2016
 *      Author: grobap
 */

#ifndef TRIHLAV_I_KEY_LIST_VIEW_HPP_
#define TRIHLAV_I_KEY_LIST_VIEW_HPP_

#include "trihlavLib/trihlavIView.hpp"

namespace trihlav {

class IButton;

class IKeyListView: virtual public IView {
public:
	virtual IButton&  getBtnAddKey() =0;
};

} /* namespace trihlav */

#endif /* TRIHLAV_I_KEY_LIST_VIEW_HPP_ */