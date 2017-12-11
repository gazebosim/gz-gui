/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef IGNITION_GUI_PLUGINS_PLOT_EXPORTDIALOG_HH_
#define IGNITION_GUI_PLUGINS_PLOT_EXPORTDIALOG_HH_

#include <list>
#include <memory>

#include "ignition/gui/qt.h"

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
  /// \brief Supported file types
  enum FileType
  {
    /// \brief Comma separated value (CSV)
    CSVFile,

    /// \brief Portable document format (PDF)
    PDFFile
  };

  // Forward declarations.
  class ExportDialogPrivate;
  class Canvas;

  /// \class ExportDialog gui/ExportDialog.hh
  /// \brief Dialog for saving to file.
  class ExportDialog : public QDialog
  {
    Q_OBJECT

    /// \brief Constructor
    /// \param[in] _parent Parent QWidget.
    /// \param[in] _plots Plots to export.
    public: ExportDialog(QWidget *_parent,
                         const std::list<Canvas *> &_plots);

    /// \brief Destructor
    public: ~ExportDialog();

    /// \brief Qt callback when the Cancel button is pressed.
    private slots: void OnCancel();

    /// \brief Qt callback when the export to csv menu is selected.
    private slots: void OnExportCSV();

    /// \brief Qt callback when the export to pdf menu is selected.
    private slots: void OnExportPDF();

    /// \brief Qt callback when a plot icon is selected.
    private slots: void OnSelected();

    /// \brief Export according to the given file type.
    /// \param[in] _type File type, such as CSV or PDF.
    private: void OnExport(FileType _type);

    /// \internal
    /// \brief Private data pointer
    private: std::unique_ptr<ExportDialogPrivate> dataPtr;
  };
}
}
}
}

#endif
