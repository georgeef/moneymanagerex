/*******************************************************
 Copyright (C) 2014 Gabriele-V

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#include "defs.h"
#include "Model_Attachment.h"
#include <wx/string.h>

Model_Attachment::Model_Attachment()
: Model<DB_Table_ATTACHMENT_V1>()
{
}

Model_Attachment::~Model_Attachment()
{
}

/**
* Initialize the global Model_Attachment table.
* Reset the Model_Attachment table or create the table if it does not exist.
*/
Model_Attachment& Model_Attachment::instance(wxSQLite3Database* db)
{
    Model_Attachment& ins = Singleton<Model_Attachment>::instance();
    ins.db_ = db;
    ins.destroy_cache();
    ins.ensure(db);

    return ins;
}

/** Return the static instance of Model_Attachment table */
Model_Attachment& Model_Attachment::instance()
{
    return Singleton<Model_Attachment>::instance();
}

/** Return a dataset with attachments linked to a specific object */
const Model_Attachment::Data_Set Model_Attachment::FilterAttachments(const wxString& RefType, const int64 RefId)
{
    Data_Set attachments;
    for (auto &attachment : this->all(COL_DESCRIPTION))
    {
        if (attachment.REFTYPE.Lower().Matches(RefType.Lower().Append("*")) && attachment.REFID == RefId)
            attachments.push_back(attachment);
    }
    return attachments;
}

/** Return the number of attachments linked to a specific object */
int Model_Attachment::NrAttachments(const wxString& RefType, const int64 RefId)
{
    return Model_Attachment::instance().find(Model_Attachment::DB_Table_ATTACHMENT_V1::REFTYPE(RefType), Model_Attachment::REFID(RefId)).size();
}

/** Return the last attachment number linked to a specific object */
int Model_Attachment::LastAttachmentNumber(const wxString& RefType, const int64 RefId)
{
    int LastAttachmentNumber = 0;
    Model_Attachment::Data_Set attachments = Model_Attachment::instance().FilterAttachments(RefType, RefId);

    for (auto &attachment : attachments)
    {
        wxString FileName = attachment.FILENAME;
        int AttachNumb = wxAtoi(FileName.SubString(FileName.Find("Attach") + 6, FileName.Find(".") - 1));
        if (AttachNumb > LastAttachmentNumber)
            LastAttachmentNumber = AttachNumb;
    }

    return LastAttachmentNumber;
}

/** Return a dataset with attachments linked to a specific type*/
std::map<int64, Model_Attachment::Data_Set> Model_Attachment::get_all(const wxString& reftype)
{
    std::map<int64, Model_Attachment::Data_Set> data;
    for (const auto & attachment : this->find(Model_Attachment::DB_Table_ATTACHMENT_V1::REFTYPE(reftype)))
    {
        data[attachment.REFID].push_back(attachment);
    }

    return data;
}

/** Return all attachments descriptions*/
wxArrayString Model_Attachment::allDescriptions()
{
    wxArrayString descriptions;
    wxString PreviousDescription;
    for (const auto &attachment : this->all(COL_DESCRIPTION))
    {
        if (attachment.DESCRIPTION != PreviousDescription)
        {
            descriptions.Add(attachment.DESCRIPTION);
            PreviousDescription = attachment.DESCRIPTION;
        }
    }
    return descriptions;
}
