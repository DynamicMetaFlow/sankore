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
#include <QList>
#include <QFileInfo>
#include <QDir>

#include "UBLibraryWidget.h"
#include "core/UBSettings.h"
#include "core/UBSetting.h"
#include "core/UBApplication.h"

#include "board/UBBoardController.h"
#include "board/UBLibraryController.h"
#include "board/UBBoardPaletteManager.h"

#include "core/UBDownloadManager.h"

#include "frameworks/UBFileSystemUtils.h"
#include "frameworks/UBPlatformUtils.h"

#include "core/memcheck.h"

/**
 * \brief Constructor
 * @param parent as the parent widget
 * @param name as the widget object name
 */
UBLibraryWidget::UBLibraryWidget(QWidget *parent, const char *name):UBThumbnailWidget(parent)
    , chainedElements(NULL)
    , mLibraryController(NULL)
    , mpCrntDir(NULL)
    , mpCrntElem(NULL)
    , mpTmpElem(NULL)
	, mLoadingLibraryItems(false)
{
    setObjectName(name);
    setSpacing(5);
    mLibraryController = new UBLibraryController(parentWidget());
}

/**
 * \brief Destructor
 */
UBLibraryWidget::~UBLibraryWidget()
{
    if(NULL != mLibraryController){
        delete mLibraryController;
        mLibraryController = NULL;
    }
    if(NULL != mpCrntDir){
        delete mpCrntDir;
        mpCrntDir = NULL;
    }
    if(NULL != mpCrntElem){
        delete mpCrntElem;
        mpCrntElem = NULL;
    }
    if(NULL != mpTmpElem){
         delete mpTmpElem;
         mpTmpElem = NULL;
    }
}

/**
 * \brief Initialize the widget content
 */
void UBLibraryWidget::init()
{
    setAcceptDrops(true);
    mpCrntElem = new UBLibElement();
    mpCrntElem->setThumbnail(QImage(":images/libpalette/home.png"));
    chainedElements = new UBChainedLibElement(mpCrntElem);
    QList<UBLibElement*> qlElems = mLibraryController->getContent(mpCrntElem);
    mCurrentElems = qlElems;

    setCurrentElemsAndRefresh(chainedElements);

    connect(this, SIGNAL(mouseClick(QGraphicsItem*,int)), this, SLOT(onItemClicked(QGraphicsItem*,int)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(UBDownloadManager::downloadManager(), SIGNAL(addDownloadedFileToLibrary(bool,QUrl,QString,QByteArray)), this, SLOT(onAddDownloadedFileToLibrary(bool,QUrl,QString,QByteArray)));
    connect(UBApplication::boardController, SIGNAL(displayMetadata(QMap<QString,QString>)), this, SLOT(onDisplayMetadata(QMap<QString,QString>)));
    connect(mLibraryController,SIGNAL(updateItemsList()),this,SLOT(onRefreshCurrentFolder()));
}

/**
 * \brief Refresh the view
 */
void UBLibraryWidget::refreshView()
{
    // Clear the view
    mItems.clear();
    mLabels.clear();
    mItemsPaths.clear();
    mGraphicItems.clear();

    // Generate the graphics items
    generateItems();

    // Set the new items
    setGraphicsItems(mGraphicItems, mItemsPaths, mLabels);

    // Refresh the view
    refreshScene();

    emit navigBarUpdate(mpCrntElem);

    bool bFavorite = false;
    if(NULL != mpCrntDir && mLibraryController->favoritePath() == mpCrntDir->path().toLocalFile())
    {
        bFavorite = true;
    }
    emit favoritesEntered(bFavorite);
}

/**
 * \brief Generate the graphic items related to the current directory
 */
void UBLibraryWidget::generateItems()
{
    for(int i = 0; i < mCurrentElems.size(); i++)
    {
        UBLibElement* pElem = mCurrentElems.at(i);
        mLabels << pElem->name();
        mItemsPaths << pElem->path();
        QGraphicsPixmapItem *pixmapItem = new UBThumbnailPixmap(QPixmap::fromImage(*pElem->thumbnail()));
        mGraphicItems << pixmapItem;
    }
}

/**
 * \brief Handles the click on an item
 * @param item as the clicked item
 * @param index as the given index
 */
void UBLibraryWidget::onItemClicked(QGraphicsItem *item, int index)
{
    Q_UNUSED(index);
    if(NULL != item)
    {
        mLoadingLibraryItems = true;
		int iItem = mGraphicItems.indexOf(item);
        if(0 <= iItem)
        {
            UBLibElement* pElem = mCurrentElems.at(iItem);
            if(NULL != pElem)
            {
                delete mpCrntElem;
                mpCrntElem = new UBLibElement(pElem);
                if(eUBLibElementType_Folder == pElem->type() || eUBLibElementType_VirtualFolder == pElem->type())
                {
                    // Add the clicked element to the end of the elements list
                    // (at this level, the user can only go down in the path)
                    UBChainedLibElement* pNextElem = new UBChainedLibElement(pElem);
                    appendChainedElement(pNextElem, chainedElements);
                    delete mpCrntDir;
                    mpCrntDir = new UBLibElement(pElem);
                    // Display the content of the folder
                    QList<UBLibElement*> qlElems = mLibraryController->getContent(mpCrntDir);
                    mCurrentElems = qlElems;
                    refreshView();
                }
                else
                {
                    if ("application/search" == UBFileSystemUtils::mimeTypeFromFileName(pElem->path().toLocalFile()))
                    {
                        emit displaySearchEngine(pElem);
                    }
                    else
                    {
                        // Display the properties view
                        emit propertiesRequested(pElem);
                    }
                }
            }
            emit itemClicked();
        }
		mLoadingLibraryItems = false;
    }
}

/**
 * \brief Append the given element to the given chain
 * @param element as the element to append
 * @param toElem as the given chain
 */
void UBLibraryWidget::appendChainedElement(UBChainedLibElement *element, UBChainedLibElement *toElem)
{
    if(NULL != toElem)
    {
        if(NULL != toElem->nextElement())
        {
            appendChainedElement(element, toElem->nextElement());
        }
        else
        {
            toElem->setNextElement(element);
        }
    }
}

/**
 * \brief Set the current element and refresh the scene
 * @param elem as the current element
 */
void UBLibraryWidget::setCurrentElemsAndRefresh(UBChainedLibElement *elem)
{
    if(NULL != elem)
    {
        UBLibElement* pLibElem = elem->element();
        if(NULL != pLibElem)
        {
            if(eUBLibElementType_Item != pLibElem->type())
            {
                QList<UBLibElement*> qlElements = mLibraryController->getContent(pLibElem);
                mCurrentElems = qlElements;
                delete mpCrntElem;
                mpCrntElem = new UBLibElement(pLibElem);
                refreshView();
                delete mpCrntDir;
                mpCrntDir = new UBLibElement(pLibElem);
                bool bFavorite = false;
                if(NULL != mpCrntDir && mLibraryController->favoritePath() == mpCrntDir->path().toLocalFile())
                {
                    bFavorite = true;
                }
                emit favoritesEntered(bFavorite);
            }
        }
    }
}

/**
 * \brief Handles the selection changed event
 */
void UBLibraryWidget::onSelectionChanged()
{
    // Get the selected items
    QList<UBLibElement*> qlSelectedItems;
    QList<QGraphicsItem*> qlGI = selectedItems();

    bCanDrag = true;
    foreach(QGraphicsItem* it, qlGI)
    {
        int itIndex = mGraphicItems.indexOf(it);
        if(0 <= itIndex)
        {
            UBLibElement* pElem = mCurrentElems.at(itIndex);
            if(NULL != pElem)
            {
                if(eUBLibElementType_Category != pElem->type() && eUBLibElementType_VirtualFolder != pElem->type()) {
                    qlSelectedItems << pElem;
                }

                if(!pElem->isMoveable())
                {
                    bCanDrag = false;
                }
            }
        }
    }

    // Check if we are in the trash folder
    bool bInTrash = false;
    if(NULL != mpCrntDir)
    {
        if("Trash" == mpCrntDir->name())
        {
            bInTrash = true;
        }
    }

    // Send the signal with these items
    emit itemsSelected(qlSelectedItems, bInTrash);
}

/**
 * \brief Handle the delete done event
 */
void UBLibraryWidget::onRefreshCurrentFolder()
{
    // Refresh the current view
    mCurrentElems = mLibraryController->getContent(mpCrntDir);
    refreshView();
}

/**
 * \brief Handles the drag enter event
 * @param event as the drag enter event
 */
void UBLibraryWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

/**
 * \brief Handles the drag move event
 * @param event as the drag move event
 */
void UBLibraryWidget::dragMoveEvent(QDragMoveEvent *event)
{
    UBLibElement* pElem = elementAt(event->pos());
    if(NULL != pElem)
    {
        // We can only drop an item into a folder
        if(eUBLibElementType_Folder == pElem->type() ||
           eUBLibElementType_VirtualFolder == pElem->type())
        {
            event->acceptProposedAction();
        }
    }
}

void UBLibraryWidget::onDropMe(const QMimeData *_data)
{
	Q_UNUSED(_data);
}

/**
 * \brief Handles the drop event
 * @param event as the drop event
 */
void UBLibraryWidget::dropEvent(QDropEvent *event)
{
    const QMimeData* pMimeData = event->mimeData();
    if(event->source() == this){
        event->accept();

        // Get the destination item
        UBLibElement* pElem = elementAt(event->pos());
        if(NULL != pElem){
            if(eUBLibElementType_Folder == pElem->type()){
                // The drag comes from this application, we have now to get the list of UBLibElements*
                QList<QString> qlDroppedElems;

                foreach(QUrl url, pMimeData->urls()){
                    qlDroppedElems << url.toString();
                }

                if(!qlDroppedElems.empty())
                    onElementsDropped(qlDroppedElems, pElem);
            }
        }
    }
    else{
        bool bDropAccepted = false;

        //  We must check the URLs first because an image dropped from the web can contains the image datas, as well as the URLs
        //  and if we want to display the download widget in order to make the user wait for the end of the download, we need
        //  to check the URLs first!
        if (pMimeData->hasUrls()){
            QList<QUrl> urlList = pMimeData->urls();
            for (int i = 0; i < urlList.size() && i < 32; ++i){
                QString filePath;
                QString crntPath = urlList.at(i).toString();

                if(crntPath.startsWith("file:") || crntPath.startsWith("/")){
                    filePath = QUrl(crntPath).toLocalFile();
                }else{
                    filePath = crntPath;
                }

                mLibraryController->importItemOnLibrary(filePath);
                bDropAccepted = true;
            }
        }
        //  When an HTML is present, it means that we dropped something from the web. Normally, the HTML contains the element
        //  of the webpage and has a 'src' attribute containing the URL of the web ressource. Here we are looking for this
        //  'src' attribute, get its value and download the ressource from this URL.
        if (!bDropAccepted && pMimeData->hasHtml()){
            QString html = pMimeData->html();
            QString url = UBApplication::urlFromHtml(html);
            if("" != url){
                mLibraryController->importItemOnLibrary(url);
                bDropAccepted = true;
            }
        }
        if (!bDropAccepted && pMimeData->hasText()){
            // On linux external dragged element are considered as text;
            QString filePath = QUrl(pMimeData->text()).toLocalFile();
            if("" != filePath){
                mLibraryController->importItemOnLibrary(filePath);
                bDropAccepted = true;
            }
            else{
#ifdef Q_WS_MACX
                //  With Safari, in 95% of the drops, the mime datas are hidden in Apple Web Archive pasteboard type.
                //  This is due to the way Safari is working so we have to dig into the pasteboard in order to retrieve
                //  the data.
                QString qsUrl = UBPlatformUtils::urlFromClipboard();
                if("" != qsUrl){
                    // We finally got the url of the dropped ressource! Let's import it!
                    mLibraryController->importItemOnLibrary(qsUrl);
                    bDropAccepted = true;
                }
#endif
            }
        }
        if (!bDropAccepted && pMimeData->hasImage()){
            QImage image = qvariant_cast<QImage>(pMimeData->imageData());
            mLibraryController->importImageOnLibrary(image);
            bDropAccepted = true;
        }

        if(bDropAccepted){
            onRefreshCurrentFolder();
#ifdef Q_WS_MACX
            event->acceptProposedAction();
#else
            event->accept();
#endif
        }
        else{
            event->ignore();
        }
    }
}

/**
 * \brief Get the element at the given position
 * @param p as the given position
 * @return a pointer on the related element
 */
UBLibElement* UBLibraryWidget::elementAt(QPoint p)
{
    QGraphicsItem* pItem = itemAt(p);
    if(NULL != pItem)
    {
        int iItem = mGraphicItems.indexOf(pItem);
        if(-1 != iItem)
        {
            return mCurrentElems.at(iItem);
        }
    }

    // If no element is found, return NULL
    return NULL;
}


/**
 * \brief Get the element from the given name
 * @param name as the given element name
 * @return the UBLibElement related to the given name
 */
UBLibElement* UBLibraryWidget::elementFromFilePath(const QString &filePath)
{
    UBLibElement* pElem = NULL;

    foreach(UBLibElement* elem, mCurrentElems)
    {
        if(elem->path().toLocalFile() == QUrl(filePath).toLocalFile())
        {
            return elem;
        }

    }

    return pElem;
}


/**
 * \brief Update the thumbnails size
 * @param newSize as the thumbnail size
 */
void UBLibraryWidget::updateThumbnailsSize(int newSize)
{
    setThumbnailWidth(newSize);
    refreshView();
}

/**
 * \brief Handles the element dropped event
 * @param elements as the list of dropped elements
 * @param target as the drop target
 */
void UBLibraryWidget::onElementsDropped(QList<QString> elements, UBLibElement *target)
{
    if(target != mpCrntDir)
    {
        QList<UBLibElement*> qlElements;

        foreach(QString qsElem, elements)
            qlElements << elementFromFilePath(qsElem);

        mLibraryController->moveContent(qlElements, target);
        mCurrentElems = mLibraryController->getContent(mpCrntDir);
        refreshView();
    }
}

/**
 * \brief Search the element related to the given text
 * @param elem as the searched element name
 */
void UBLibraryWidget::onSearchElement(QString elem)
{
    // Store the original list of items
    mOrigCurrentElems = mLibraryController->getContent(mpCrntDir);

    // Build the filtered list
    mCurrentElems.clear();
    if(elem.isEmpty())
    {
        mCurrentElems = mOrigCurrentElems;
    }
    else
    {
        foreach(UBLibElement* ubLibElem, mOrigCurrentElems)
        {
            if(ubLibElem->name().toLower().contains(elem.toLower()))
            {
                mCurrentElems << ubLibElem;
            }
        }
    }
    refreshView();
}

/**
 * \brief Create a new folder
 */
void UBLibraryWidget::onNewFolderToCreate()
{
    // Create here a dialog asking the name of the new folder
    UBNewFolderDlg dlg;
    if(QDialog::Accepted == dlg.exec())
    {
        mLibraryController->createNewFolder(dlg.folderName(), mpCrntElem);
        onRefreshCurrentFolder();
    }
}

/**
 * \brief Constructor
 * @param parent as the parent widget
 * @param name as the object name
 */
UBNewFolderDlg::UBNewFolderDlg(QWidget *parent, const char *name):QDialog(parent)
    , mpLabel(NULL)
    , mpLineEdit(NULL)
    , mpButtons(NULL)
    , mpAddButton(NULL)
    , mpCancelButton(NULL)
    , mpLayout(NULL)
    , mpHLayout(NULL)
{
    setObjectName(name);
    setWindowTitle(tr("Add new folder"));

    mpLabel = new QLabel(tr("New Folder name:"),this);
    mpLineEdit = new QLineEdit(this);
    mpAddButton = new QPushButton(tr("Add"));
    mpAddButton->setDefault(true);

    mpCancelButton = new QPushButton(tr("Cancel"));
    mpCancelButton->setAutoDefault(false);

    mpButtons = new QDialogButtonBox(Qt::Horizontal, this);
    mpLayout = new QVBoxLayout(this);
    mpHLayout = new QHBoxLayout(this);
    setLayout(mpLayout);
    mpLayout->addLayout(mpHLayout, 0);
    mpHLayout->addWidget(mpLabel, 0);
    mpHLayout->addWidget(mpLineEdit, 1);

    mpButtons->addButton(mpAddButton,QDialogButtonBox::ActionRole);
    mpButtons->addButton(mpCancelButton,QDialogButtonBox::ActionRole);
    mpLayout->addWidget(mpButtons);

    connect(mpAddButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(mpCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(mpLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(text_Changed(const QString &)));
    connect(mpLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(text_Edited(const QString &)));

    setMaximumHeight(100);
    setMinimumHeight(100);
}

/**
 * \brief Destructor
 */
UBNewFolderDlg::~UBNewFolderDlg()
{
    if(NULL != mpAddButton)
    {
        delete mpAddButton;
        mpAddButton = NULL;
    }

    if(NULL != mpCancelButton)
    {
        delete mpCancelButton;
        mpCancelButton = NULL;
    }
    if(NULL != mpButtons)
    {
        delete mpButtons;
        mpButtons = NULL;
    }
    if(NULL != mpLineEdit)
    {
        delete mpLineEdit;
        mpLineEdit = NULL;
    }
    if(NULL != mpLabel)
    {
        delete mpLabel;
        mpLabel = NULL;
    }
    if(NULL != mpHLayout)
    {
        delete mpHLayout;
        mpHLayout = NULL;
    }
    if(NULL != mpLayout)
    {
        delete mpLayout;
        mpLayout = NULL;
    }
}

/**
 * \brief Get the folder name
 * @return the entered folder name
 */
QString UBNewFolderDlg::folderName()
{
    return mpLineEdit->text();
}

void UBNewFolderDlg::text_Changed(const QString &newText)
{
	Q_UNUSED(newText);
}

/*
http://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx

< (less than)
> (greater than)
: (colon)
" (double quote)
/ (forward slash)
\ (backslash) // Note: The C++ compiler transforms backslashes in strings. To include a \ in a regexp, enter it twice, i.e. \\. To match the backslash character itself, enter it four times, i.e. \\\\.
| (vertical bar or pipe)
? (question mark)
* (asterisk)

*/

void UBNewFolderDlg::text_Edited(const QString &newText)
{

    QString new_text = newText;

#ifdef Q_WS_WIN // Defined on Windows.
    QString illegalCharList("      < > : \" / \\ | ? * ");
    QRegExp regExp("[<>:\"/\\\\|?*]");
#endif

#ifdef Q_WS_QWS // Defined on Qt for Embedded Linux.
    QString illegalCharList("      < > : \" / \\ | ? * ");
    QRegExp regExp("[<>:\"/\\\\|?*]");
#endif

#ifdef Q_WS_MAC // Defined on Mac OS X.
    QString illegalCharList("      < > : \" / \\ | ? * ");
    QRegExp regExp("[<>:\"/\\\\|?*]");
#endif

#ifdef Q_WS_X11 // Defined on X11.
    QString illegalCharList("      < > : \" / \\ | ? * ");
    QRegExp regExp("[<>:\"/\\\\|?*]");
#endif

    if(new_text.indexOf(regExp) > -1)
    {
        new_text.remove(regExp);
        mpLineEdit->setText(new_text);
        QToolTip::showText(mpLineEdit->mapToGlobal(QPoint()), "A file name can`t contain any of the following characters:\r\n"+illegalCharList);
    }
}

void UBLibraryWidget::onAddDownloadedFileToLibrary(bool pSuccess, QUrl sourceUrl, QString pContentHeader, QByteArray pData)
{
    Q_UNUSED(pContentHeader);
    if(pSuccess)
    {
//        QDir dir;
//        dir.mkdir("tmp");
//        QString qsFileName = QFileInfo(sourceUrl.toString()).fileName();
//        QString qsFilePath = UBFileSystemUtils::normalizeFilePath(QString("tmp/%0").arg(qsFileName));
//        QFile f(qsFilePath);
//        if(f.open(QIODevice::WriteOnly))
//        {
//            f.write(pData);
//            f.close();
//        }
        QString urlString = sourceUrl.toString();
        mLibraryController->routeDataItem(urlString, pData);
//        dir.remove(qsFileName);
//        dir.rmdir("tmp");       // Due to Qt, the directoy will be removed only if it's empty :)
    }
}

void UBLibraryWidget::onDisplayMetadata(QMap<QString, QString> metadatas)
{
    mpTmpElem = new UBLibElement();
    mpTmpElem->setMetadata(metadatas);
    mpTmpElem->setPath(QUrl(metadatas["Url"]));

    // As the content comes from the web (and need a download), we will not display its thumbnail.
    mpTmpElem->setThumbnail(QImage(":images/libpalette/notFound.png"));

    // Display the properties view
    emit propertiesRequested(mpTmpElem);
}
