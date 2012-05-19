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

#include "UBWidgetMessageAPI.h"

#include "core/UBApplication.h"

#include "domain/UBAbstractWidget.h"

#include "core/memcheck.h"

UBWidgetMessageAPI::UBWidgetMessageAPI(UBW3CWidget *widget)
    : QObject(widget)
    , mWebWidget(widget)
{
    connect(UBWidgetAPIMessageBroker::instance(), SIGNAL(newMessage(const QString&, const QString&))
            , this, SLOT(onNewMessage(const QString&, const QString&)), Qt::QueuedConnection);
}

UBWidgetMessageAPI::~UBWidgetMessageAPI()
{
    // NOOP
}


void UBWidgetMessageAPI::sendMessage(const QString& pTopicName, const QString& pMessage)
{
    UBWidgetAPIMessageBroker::instance()->sendMessage(pTopicName, pMessage);
}


void UBWidgetMessageAPI::onNewMessage(const QString& pTopicName, const QString& pMessage)
{
    if (mSubscribedTopics.contains(pTopicName))
    {
        if (mWebWidget
                && mWebWidget->page()
                && mWebWidget->page()->mainFrame())
        {

            QString js;
            js += "if(widget && widget.messages && widget.messages.onmessage)";
            js += "{widget.messages.onmessage('";
            js += pMessage + "', '" + pTopicName + "')}";

            mWebWidget->page()->
                mainFrame()->evaluateJavaScript(js);

        }
    }
}



UBWidgetAPIMessageBroker* UBWidgetAPIMessageBroker::sInstance = 0;


UBWidgetAPIMessageBroker::UBWidgetAPIMessageBroker(QObject *parent)
    : QObject(parent)
{
    // NOOP
}


UBWidgetAPIMessageBroker::~UBWidgetAPIMessageBroker()
{
    // NOOP
}


UBWidgetAPIMessageBroker* UBWidgetAPIMessageBroker::instance()
{
    if (!sInstance)
        sInstance = new UBWidgetAPIMessageBroker(UBApplication::staticMemoryCleaner);

    return sInstance;

}


void UBWidgetAPIMessageBroker::sendMessage(const QString& pTopicName, const QString& pMessage)
{
    emit newMessage(pTopicName, pMessage);
}
