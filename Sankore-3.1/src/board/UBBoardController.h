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

#ifndef UBBOARDCONTROLLER_H_
#define UBBOARDCONTROLLER_H_

#include <QtGui>

#include <QObject>

class UBMainWindow;
class UBApplication;
class UBBoardView;

class UBDocumentController;
class UBMessageWindow;
class UBGraphicsScene;
class UBDocumentProxy;
class UBBlackoutWidget;
class UBToolWidget;
class UBVersion;
class UBSoftwareUpdate;
class UBSoftwareUpdateDialog;
class UBGraphicsVideoItem;
class UBGraphicsAudioItem;
class UBGraphicsWidgetItem;
class UBBoardPaletteManager;


class UBBoardController : public QObject
{
    Q_OBJECT

    public:
        UBBoardController(UBMainWindow *mainWindow);
        virtual ~UBBoardController();

        void init();
        void setupLayout();
        UBDocumentProxy* activeDocument() const;
        UBGraphicsScene* activeScene() const;
        int activeSceneIndex() const;
        QSize displayViewport();
        QSize controlViewport();
        QRectF controlGeometry();
        void closing();

        int currentPage();

        int pageFromSceneIndex(int sceneIndex);
        int sceneIndexFromPage(int page);

        UBDocumentProxy* activeDocument()
        {
            return mActiveDocument;
        }

        QWidget* controlContainer()
        {
            return mControlContainer;
        }

        UBBoardView* controlView()
        {
            return mControlView;
        }

        UBBoardView* displayView()
        {
            return mDisplayView;
        }

        UBGraphicsScene* activeScene()
        {
            return mActiveScene;
        }

        void setPenColorOnDarkBackground(const QColor& pColor)
        {
            if (mPenColorOnDarkBackground == pColor)
                return;

            mPenColorOnDarkBackground = pColor;
            emit penColorChanged();
        }

        void setPenColorOnLightBackground(const QColor& pColor)
        {
            if (mPenColorOnLightBackground == pColor)
                return;

            mPenColorOnLightBackground = pColor;
            emit penColorChanged();
        }

        void setMarkerColorOnDarkBackground(const QColor& pColor)
        {
            mMarkerColorOnDarkBackground = pColor;
        }

        void setMarkerColorOnLightBackground(const QColor& pColor)
        {
            mMarkerColorOnLightBackground = pColor;
        }

        QColor penColorOnDarkBackground()
        {
            return mPenColorOnDarkBackground;
        }

        QColor penColorOnLightBackground()
        {
            return mPenColorOnLightBackground;
        }

        QColor markerColorOnDarkBackground()
        {
            return mMarkerColorOnDarkBackground;
        }

        QColor markerColorOnLightBackground()
        {
            return mMarkerColorOnLightBackground;
        }

        qreal systemScaleFactor()
        {
            return mSystemScaleFactor;
        }
        qreal currentZoom();
        void persistCurrentScene();
        void showNewVersionAvailable(bool automatic, const UBVersion &installedVersion, const UBSoftwareUpdate &softwareUpdate);
        void setBoxing(QRect displayRect);
        void setToolbarTexts();
        static QUrl expandWidgetToTempDir(const QByteArray& pZipedData, const QString& pExtension = QString("wgt"));
//        static QRect freeRectInGlobalPos() const {return ;}
        void setPageSize(QSize newSize);
        UBBoardPaletteManager *paletteManager()
        {
            return mPaletteManager;
        }

        void notifyCache(bool visible);
        void notifyPageChanged();
        void displayMetaData(QMap<QString, QString> metadatas);

        void ClearUndoStack();
        void emitScrollSignal() { emit scrollToSelectedPage(); }

    public slots:
        void setActiveDocumentScene(UBDocumentProxy* pDocumentProxy, int pSceneIndex = 0);
        void showDocumentsDialog();
        void showKeyboard(bool show);
        void togglePodcast(bool checked);
        void blackout();
        void addScene();
        void addScene(UBDocumentProxy* proxy, int sceneIndex, bool replaceActiveIfEmpty = false);
        void addScene(UBGraphicsScene* scene, bool replaceActiveIfEmpty = false);
        void duplicateScene();
        void importPage();
        void clearScene();
        void clearSceneItems();
        void clearSceneAnnotation();
        void zoomIn(QPointF scenePoint = QPointF(0,0));
        void zoomOut(QPointF scenePoint = QPointF(0,0));
        void zoomRestore();
        void centerRestore();
        void centerOn(QPointF scenePoint = QPointF(0,0));
        void zoom(const qreal ratio, QPointF scenePoint);
        void handScroll(qreal dx, qreal dy);
        void previousScene();
        void nextScene();
        void firstScene();
        void lastScene();
        void downloadURL(const QUrl& url, const QPointF& pPos = QPointF(0.0, 0.0), const QSize& pSize = QSize(), bool isBackground = false);
        void downloadFinished(bool pSuccess, QUrl sourceUrl, QString pHeader, QByteArray pData, QPointF pPos, QSize pSize, bool isBackground = false);
        void changeBackground(bool isDark, bool isCrossed);
        void setToolCursor(int tool);
        void showMessage(const QString& message, bool showSpinningWheel);
        void hideMessage();
        void setDisabled(bool disable);
        void setColorIndex(int pColorIndex);
        UBToolWidget* addTool(const QUrl& toolUrl, QPointF scenePos);
        UBToolWidget* addTool(const QUrl& toolUrl);
        void removeTool(UBToolWidget* toolWidget);
        void hide();
        void show();
        void setWidePageSize(bool checked);
        void setRegularPageSize(bool checked);
        void stylusToolChanged(int tool);
        void grabScene(const QRectF& pSceneRect);
        void controlViewHidden();
        void controlViewShown();
        UBGraphicsVideoItem* addVideo(const QUrl& pUrl, bool startPlay, const QPointF& pos);
        UBGraphicsAudioItem* addAudio(const QUrl& pUrl, bool startPlay, const QPointF& pos);
        UBGraphicsWidgetItem *addW3cWidget(const QUrl& pUrl, const QPointF& pos);

        void cut();
        void copy();
        void paste();
        void processMimeData(const QMimeData* pMimeData, const QPointF& pPos);
        void moveGraphicsWidgetToControlView(UBGraphicsWidgetItem* graphicWidget);
        void moveToolWidgetToScene(UBToolWidget* toolWidget);
        void addItem();

        void freezeW3CWidgets(bool freeze);
        void freezeW3CWidget(QGraphicsItem* item, bool freeze);
        void startScript();
        void stopScript();

    signals:
        void activeSceneWillBePersisted();
        void activeSceneWillChange();
        void activeSceneChanged();
        void activeDocumentChanged();
        void zoomChanged(qreal pZoomFactor);
        void systemScaleFactorChanged(qreal pSystemScaleFactor);
        void penColorChanged();
        void controlViewportChanged();
        void backgroundChanged();
        void cacheEnabled();
        void cacheDisabled();
        void pageChanged();
        void setDocOnPageNavigator(UBDocumentProxy* doc);
        void documentReorganized(int index);
        void displayMetadata(QMap<QString, QString> metadata);
        void scrollToSelectedPage();

    protected:
        void setupViews();
        void setupToolbar();
        void connectToolbar();
        void initToolbarTexts();
        void updateActionStates();
        void updateSystemScaleFactor();
        QString truncate(QString text, int maxWidth);

    protected slots:
        void selectionChanged();
        void undoRedoStateChange(bool canUndo);
        void documentSceneChanged(UBDocumentProxy* proxy, int pIndex);

    private:
        void updatePageSizeState();
        void saveViewState();
        void adjustDisplayViews();

        UBMainWindow *mMainWindow;
        UBDocumentProxy* mActiveDocument;
        UBGraphicsScene* mActiveScene;
        int mActiveSceneIndex;
        UBBoardPaletteManager *mPaletteManager;
        UBSoftwareUpdateDialog *mSoftwareUpdateDialog;
        UBMessageWindow *mMessageWindow;
        UBBoardView *mControlView;
        UBBoardView *mDisplayView;
        QWidget *mControlContainer;
        QHBoxLayout *mControlLayout;
        qreal mZoomFactor;
        bool mIsClosing;
        QColor mPenColorOnDarkBackground;
        QColor mPenColorOnLightBackground;
        QColor mMarkerColorOnDarkBackground;
        QColor mMarkerColorOnLightBackground;
        QList<UBToolWidget*> mTools;
        qreal mSystemScaleFactor;
        bool mCleanupDone;
        QMap<QAction*, QPair<QString, QString> > mActionTexts;

    private slots:
        void stylusToolDoubleClicked(int tool);
        void boardViewResized(QResizeEvent* event);
        void documentWillBeDeleted(UBDocumentProxy* pProxy);
        void updateBackgroundActionsState(bool isDark, bool isCrossed);
        void updateBackgroundState();
        void colorPaletteChanged();
        void libraryDialogClosed(int ret);
        void lastWindowClosed();
        void onDownloadModalFinished();

};


#endif /* UBBOARDCONTROLLER_H_ */
