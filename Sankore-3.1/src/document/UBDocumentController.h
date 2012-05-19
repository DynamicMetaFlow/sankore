/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UBDOCUMENTCONTROLLER_H_
#define UBDOCUMENTCONTROLLER_H_

#include <QtGui>

namespace Ui
{
    class documents;
}

#include "gui/UBMessageWindow.h"

class UBGraphicsScene;
class QDialog;
class UBDocumentProxy;
class UBBoardController;
class UBThumbnailsScene;
class UBDocumentGroupTreeItem;
class UBDocumentProxyTreeItem;
class UBMainWindow;
class UBDocumentToolsPalette;

class UBDocumentController : public QObject
{
    Q_OBJECT;

    public:
        UBDocumentController(UBMainWindow* mainWindow);
        virtual ~UBDocumentController();

        void closing();
        QWidget* controlView();
        UBDocumentProxyTreeItem* findDocument(UBDocumentProxy* proxy);
        bool addFileToDocument(UBDocumentProxy* document);
        UBDocumentProxy* getCurrentDocument() { return mCurrentDocument; };
        void deletePages(QList<QGraphicsItem*> itemsToDelete);
        int getSelectedItemIndex();

    signals:
        void refreshThumbnails();
        void exportDone();
        void movedToIndex(int index);

    public slots:
        void createNewDocument();
        void createNewDocumentGroup();
        void deleteSelectedItem();
        void renameSelectedItem();
        void openSelectedItem();
        void duplicateSelectedItem();
        void importFile();
        void moveSceneToIndex(UBDocumentProxy* proxy, int source, int target);
        void selectDocument(UBDocumentProxy* proxy, bool setAsCurrentDocument = true);
        void show();
        void hide();
        void showMessage(const QString& message, bool showSpinningWheel);
        void hideMessage();
        void toggleDocumentToolsPalette();
        void cut();
        void copy();
        void paste();
        void focusChanged(QWidget *old, QWidget *current);
        void reloadThumbs();

    protected:
        virtual void setupViews();
        virtual void setupToolbar();
        void setupPalettes();
        bool isOKToOpenDocument(UBDocumentProxy* proxy);
        UBGraphicsScene* activeScene();
        UBDocumentProxy* selectedDocumentProxy();
        UBDocumentProxyTreeItem* selectedDocumentProxyTreeItem();
        UBDocumentGroupTreeItem* selectedDocumentGroupTreeItem();
        QStringList allGroupNames();

        enum LastSelectedElementType
        {
            None = 0, Folder, Document, Page
        };

        LastSelectedElementType mSelectionType;

    private:
        QWidget *mParentWidget;
        UBBoardController *mBoardController;
        Ui::documents* mDocumentUI;
        UBMainWindow* mMainWindow;
        QWidget *mDocumentWidget;
        QPointer<UBMessageWindow> mMessageWindow;
        QAction* mAddFolderOfImagesAction;
        QAction* mAddFileToDocumentAction;
        QAction* mAddImagesAction;
        bool mIsClosing;
        UBDocumentToolsPalette *mToolsPalette;
        bool mToolsPalettePositionned;
        UBDocumentGroupTreeItem* mTrashTi;
        UBDocumentProxy* mCurrentDocument;
        QList<QPixmap> mDocumentThumbs;

    private slots:
        void documentZoomSliderValueChanged (int value);
        void loadDocumentProxies();
        void itemSelectionChanged();
        void refreshDocumentThumbnailsView();
        void exportDocument();
        void itemChanged(QTreeWidgetItem * item, int column);
        void thumbnailViewResized();
        void pageSelectionChanged();
        void selectionChanged();
        void documentSceneChanged(UBDocumentProxy* proxy, int pSceneIndex);
        void pageDoubleClicked(QGraphicsItem* item, int index);
        void pageClicked(QGraphicsItem* item, int index);
        void itemClicked(QTreeWidgetItem * item, int column );
        void addToDocument();
        void addDocumentInTree(UBDocumentProxy* pDocument);
        void updateDocumentInTree(UBDocumentProxy* pDocument);
        void addFolderOfImages();
        void addFileToDocument();
        void addImages();

};



#endif /* UBDOCUMENTCONTROLLER_H_ */
