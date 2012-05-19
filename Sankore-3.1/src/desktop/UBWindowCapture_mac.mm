
#include "UBWindowCapture.h"
#include "UBDesktopAnnotationController.h"

#import <Foundation/NSAutoreleasePool.h>
#import <Carbon/Carbon.h>


UBWindowCapture::UBWindowCapture(UBDesktopAnnotationController *parent) 
	: QObject(parent)
	, mParent(parent)
{
	// NOOP
}


UBWindowCapture::~UBWindowCapture() 
{
	// NOOP
}


const QPixmap UBWindowCapture::getCapturedWindow() 
{
	return mWindowPixmap;
}


int UBWindowCapture::execute()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSString *path = [NSString pathWithComponents: [NSArray arrayWithObjects: NSTemporaryDirectory(), 
			[[NSString stringWithFormat:@"%d",[NSDate timeIntervalSinceReferenceDate]] stringByAppendingPathExtension:@"uninote"], 
			nil]];
	
    NSTask *task = [[NSTask alloc] init];
	NSArray *arguments = [NSArray arrayWithObjects: @"-i", @"-W", @"-m", @"-tpng", path, nil];
    [task setLaunchPath: @"/usr/sbin/screencapture"];
    [task setArguments: arguments];
	
    [task launch];
    [task waitUntilExit];
    [task release];
	
    QString resultPath = QString::fromUtf8([path UTF8String], strlen([path UTF8String]));
	
    mWindowPixmap.load(resultPath);
    
    QFile::remove(resultPath);
    
    [pool drain];
    
    return QDialog::Accepted;
}

