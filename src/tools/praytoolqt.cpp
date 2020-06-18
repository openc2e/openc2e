/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "tools/praytoolqt.h"
#include "fileformats/Caos2PrayParser.h"
#include "fileformats/PraySourceParser.h"
#include "fileformats/PrayFileWriter.h"
#include "visit_overloads.h"

#include <QApplication>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QLabel>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include "backends/qtgui/invokeMethod.h"


#include <fstream>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

PrayToolQt::PrayToolQt()
{
    dropArea = new QLabel;
    dropArea->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    dropArea->setAlignment(Qt::AlignCenter);
    dropArea->setAutoFillBackground(true);
    dropArea->setFixedHeight(50); // TODO: base off font size

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(dropArea);
    
    plain_text_edit = new QPlainTextEdit;
    plain_text_edit->setReadOnly(true);
    plain_text_edit->setPlaceholderText(tr("Waiting for files..."));
    mainLayout->addWidget(plain_text_edit);
    
    setWindowTitle(tr("Openc2e Resource Tool"));
    setMinimumSize(400, 50);
    setAcceptDrops(true);
    
    setState(STATE_DEFAULT);
}

PrayToolQt::~PrayToolQt()
{
    if (work_thread.joinable()) {
        work_thread.join();
    }
}

void PrayToolQt::setState(WindowState state_) {
    state = state_;
    switch (state) {
        case STATE_DEFAULT:
            dropArea->setBackgroundRole(QPalette::AlternateBase);
            dropArea->setDisabled(false);
            dropArea->setText(tr("Drop files here"));
            return;
        case STATE_DRAGGING:
            dropArea->setBackgroundRole(QPalette::Dark);
            dropArea->setDisabled(false);
            dropArea->setText(tr("Drop files here"));
            return;
        case STATE_PROCESSING:
            dropArea->setBackgroundRole(QPalette::AlternateBase);
            dropArea->setDisabled(true);
            dropArea->setText("Processing files...");
            return;
    }
}

void PrayToolQt::dragEnterEvent(QDragEnterEvent *event) {
    if (state == STATE_PROCESSING) return;
    setState(STATE_DRAGGING);
    event->acceptProposedAction();
}

void PrayToolQt::dragLeaveEvent(QDragLeaveEvent *event) {
    if (state == STATE_PROCESSING) return;
    setState(STATE_DEFAULT);
    event->accept();
}

static void writeText(PrayToolQt *window, const std::string& text) {
    invokeMethod(window, [=]{
        window->plain_text_edit->appendPlainText(QString::fromStdString(text));
    });        
}

static void writeRedText(PrayToolQt *window, const std::string& text) {
    invokeMethod(window, [=]{
        window->plain_text_edit->appendHtml(QString::fromStdString("<font color=red>" + text + "</font>"));
    });        
}

static void do_pray_tool(const std::string& filename, PrayToolQt *window) {
    invokeMethod(window, [=]{
        window->plain_text_edit->setPlainText(QString::fromStdString("Processing " + filename));
    });
    
    std::ifstream f(filename);
    std::string str((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());

    fs::path parent_path = fs::path(filename).parent_path();

    std::string output_filename;
    std::vector<PraySourceParser::Event> events;
    if (fs::path(filename).extension() == ".agent" || fs::path(filename).extension() == ".agents") {
        // TODO: do praydump
    } if (fs::path(filename).extension() == ".txt") {
        events = PraySourceParser::parse(str);
    } else if (fs::path(filename).extension() == ".cos") {
        events = Caos2PrayParser::parse(str, output_filename.size() ? nullptr : &output_filename);
    } else {
        writeRedText(window, "Don't know how to handle input file \"" + filename + "\"");
        return;
    }
    if (!output_filename.size()) {
        output_filename = fs::path(filename).stem().string() + ".agents";
    }
    output_filename = fs::path(filename).parent_path() / output_filename;
    
    if (mpark::holds_alternative<PraySourceParser::Error>(events[0])) {
        writeRedText(window, "Error: "
                  + mpark::get<PraySourceParser::Error>(events[0]).message);
        return;
    }

    writeText(window, "Writing output to \"" + output_filename + "\"");
    std::ofstream out(output_filename, std::ios::binary);
    PrayFileWriter writer(out);

    std::map<std::string, std::string> string_tags;
    std::map<std::string, int> int_tags;
    
    for (auto res : events) {
      visit_overloads(
          res, [](PraySourceParser::Error) {
              /* handled already */
          },
          [&](PraySourceParser::GroupBlockStart) {
            string_tags = {};
            int_tags = {};
          },
          [&](PraySourceParser::GroupBlockEnd event) {
            writer.writeBlockTags(event.type, event.name, int_tags, string_tags);
            writeText(window, "Tag block " + event.type + " \""
                      + event.name );
          },
          [&](PraySourceParser::InlineBlock event) {
              writeText(window, "Inline block " + event.type + " \""
                      + event.name + "\" from file \"" + event.filename
                  );
            
            // TODO: check in same directory
          std::ifstream in((parent_path / event.filename).string());
          if (!in) {
              writeRedText(window, "Couldn't open file \""
                        + (parent_path / event.filename).string() + "\""
                    );
              return;
          }
          std::vector<char> data((std::istreambuf_iterator<char>(in)),
                          std::istreambuf_iterator<char>());
                          
            writer.writeBlockRawData(event.type, event.name, data.data(), data.size());
            
          },
          [&](PraySourceParser::StringTag event) {
            string_tags[event.key] = event.value;
          },
          [&](PraySourceParser::StringTagFromFile event) {
            // TODO: check in same directory
            std::ifstream in((parent_path / event.filename).string());
            if (!in) {
                writeRedText(window, "Couldn't open file \""
                          + (parent_path / event.filename).string() + "\""
                      );
                return;
            }
            std::string val((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
                            
            string_tags[event.key] = val;
          },
          [&](PraySourceParser::IntegerTag event) {
            int_tags[event.key] = event.value;
          });
    }
    
    writeText(window, "Wrote output to \"" + output_filename + "\"");
    writeText(window, "Done!");
}

static std::string get_filename_from_qmimedata(const QMimeData* mimedata)
{
    if (!mimedata->hasUrls()) {
        return "";
    }
    QList<QUrl> urllist = mimedata->urls();
    if (urllist.size() != 1) {
        return "";
    }
    std::string scheme = urllist.at(0).scheme().toStdString();
    if (scheme != "file") {
        return "";
    }
    return urllist.at(0).path().toStdString();
}

void PrayToolQt::dropEvent(QDropEvent *event) {
    if (state == STATE_PROCESSING) return;
    
    printf("drop\n");
    const QMimeData *mimeData = event->mimeData();
    std::string filename = get_filename_from_qmimedata(mimeData);
    if (filename == "") {
        plain_text_edit->setPlainText("");
        plain_text_edit->appendHtml("<font color=red>Dropped file not supported</font>");
        setState(STATE_DEFAULT);
        event->acceptProposedAction();
        return;
    }
    setState(STATE_PROCESSING);
    // plain_text_edit->setPlainText(QString::fromStdString("Processing " + filename));
    
    if (work_thread.joinable()) {
        work_thread.join();
    }
    work_thread = std::thread([=]{
        do_pray_tool(filename, this);
        setState(STATE_DEFAULT);
    });
    
    event->acceptProposedAction();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PrayToolQt window;
    window.show();
    return app.exec();
}