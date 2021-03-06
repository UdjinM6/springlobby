/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentsearchresultslistctrl.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"

template <>
SortOrder ContentSearchResultsListctrl::BaseType::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE(ContentSearchResultsListctrl, ContentSearchResultsListctrl::BaseType)

END_EVENT_TABLE()

ContentSearchResultsListctrl::ContentSearchResultsListctrl(wxWindow* parent, wxWindowID id, const wxString& /*name*/, long int /*style*/, const wxPoint& /*pt*/, const wxSize& /*sz*/)
    : ContentSearchResultsListctrl::BaseType(parent, id, wxDefaultPosition, wxDefaultSize,
					     wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("ContentSearchResultsListctrl"),
					     3, &ContentSearchResultsListctrl::CompareOneCrit)
{
	AddColumn(0, wxLIST_AUTOSIZE, _("Name"), _("Name"));
	AddColumn(1, wxLIST_AUTOSIZE, _("Preview"), _("Preview"));
	AddColumn(2, wxLIST_AUTOSIZE, _("Type"), _("Type"));
	AddColumn(3, wxLIST_AUTOSIZE, _("Size"), _("Size"));
	AddColumn(4, wxLIST_AUTOSIZE, _("Downloaded"), _("Downloaded"));

	if (m_sortorder.size() == 0) {
		m_sortorder[2].col = 2;
		m_sortorder[2].direction = 1;
		m_sortorder[0].col = 0;
		m_sortorder[0].direction = 1;
		m_sortorder[1].col = 1;
		m_sortorder[1].direction = 1;
	}
}

ContentSearchResultsListctrl::~ContentSearchResultsListctrl()
{
}

int ContentSearchResultsListctrl::CompareOneCrit(DataType u1, DataType u2, int col, int dir) const
{
	switch (col) {
		case 0: //Name
		case 2: //type
			return u1->name.CmpNoCase(u2->name) * dir;
		case 1: // preview
			return 0;
		case 3:
			return (u1->filesize - u2->filesize) * dir;
		case 4:
			return (u1->is_downloaded < u2->is_downloaded);
	}
	return -1;
}

int ContentSearchResultsListctrl::GetItemColumnImage(long int /*item*/, long int /*column*/) const
{
	return -1;
}
int ContentSearchResultsListctrl::GetItemImage(long int /*item*/) const
{
	return -1;
}
wxString ContentSearchResultsListctrl::GetItemText(long int item, long int column) const
{
	if (m_data[item] == NULL)
		return wxEmptyString;
	const ContentSearchResult* res = m_data[item];
	switch (column) {
		case 0:
			return res->name;
		case 1:
			return _("TODO");
		case 2:
			return res->type;
		case 3: {
			int kb = res->filesize / 1024;
			int mb = kb / 1024;

			if (mb > 0)
				return wxString::Format(_("%d MB"), mb);
			else if (kb > 0)
				return wxString::Format(_("%d KB"), kb);
			else
				return _("< 1 KB");
		}
		case 4: {
			if (res->is_downloaded)
				return _("Yes");
			else
				return _("No");
		}
	}
	return wxEmptyString;
}
void ContentSearchResultsListctrl::Sort()
{
}
int ContentSearchResultsListctrl::GetIndexFromData(ContentSearchResult* const& data) const
{
	static long seekpos;
	seekpos = LSL::Util::Clamp(seekpos, 0l, (long)m_data.size());
	int index = seekpos;

	for (DataCIter f_idx = m_data.begin() + seekpos; f_idx != m_data.end(); ++f_idx) {
		if (data == *f_idx) {
			seekpos = index;
			return seekpos;
		}
		index++;
	}
	//it's ok to init with seekpos, if it had changed this would not be reached
	int r_index = seekpos - 1;
	for (DataRevCIter r_idx = m_data.rbegin() + (m_data.size() - seekpos); r_idx != m_data.rend(); ++r_idx) {
		if (data == *r_idx) {
			seekpos = r_index;
			return seekpos;
		}
		r_index--;
	}

	return -1;
}

void ContentSearchResultsListctrl::AddContent(ContentSearchResult*& content)
{
	AddItem(content);
}
