#include "Utils/FIleSystem/FileSystem.h"

#import <Cocoa/Cocoa.h>

#include "Application/Application.h"
#include "Utils/Debug.h"
#include "Utils/Log/LogStream.h"

namespace o2
{
	FolderInfo FileSystem::GetFolderInfo(const String& path) const
    {
        FolderInfo res;
        res.mPath = path;
        
        NSString* searchPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + path).Data()]];
        
        @try
        {
            NSArray* dirs = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:searchPath error:NULL];
            
            for (id obj in dirs)
            {
                String filePath = path + "/" + [(NSString*)obj UTF8String];
                
                BOOL isDir = NO;
                [[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:filePath] isDirectory:&isDir];
                
                if (isDir == YES)
                    res.mFolders.Add(GetFolderInfo(filePath));
                else
                    res.mFiles.Add(GetFileInfo(filePath));
            };
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error("Failed GetPathInfo: Error opening directory " + path + ", exception:" + [[exception reason] UTF8String]);
        }
        
		return res;
	}

	bool FileSystem::FileCopy(const String& source, const String& dest) const
	{
		FileDelete(dest);
		FolderCreate(ExtractPathStr(dest));
        
        bool result = false;
        
        @try
        {
            NSString* sourcePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + source).Data()]];
            NSString* destPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + dest).Data()]];
            
            result = [[NSFileManager defaultManager] copyItemAtPath:sourcePath toPath:destPath error:NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed FileCopy, exception:" + [[exception reason] UTF8String]);
        }
        
        return result;
	}

	bool FileSystem::FileDelete(const String& file) const
    {
        bool result = false;
        
        @try
        {
            NSString* filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + file).Data()]];
            result = [[NSFileManager defaultManager] removeItemAtPath:filePath error:NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed FileDelete, exception:" + [[exception reason] UTF8String]);
        }
        
        return result;
	}

	bool FileSystem::FileMove(const String& source, const String& dest) const
	{
		String destFolder = GetParentPath(dest);

		if (!IsFolderExist(destFolder))
            FolderCreate(destFolder);
        
        bool result = false;
        
        @try
        {
            NSString* sourcePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + source).Data()]];
            NSString* destPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + dest).Data()]];
            
            result = [[NSFileManager defaultManager] moveItemAtPath:sourcePath toPath:destPath error:NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed FileMove, exception:" + [[exception reason] UTF8String]);
        }

        return result;
	}

	FileInfo FileSystem::GetFileInfo(const String& path) const
	{
		FileInfo res;
        
        res.mPath = path;
        
        String extension = path.SubStr(path.FindLast(".") + 1);
        res.mFileType = FileType::File;
        for (auto iext : mInstance->mExtensions)
        {
            if (iext.Value().Contains(extension))
            {
                res.mFileType = iext.Key();
                break;
            }
        }
        
        NSDictionary* fileAttribs;
        
        @try
        {
            NSString* npath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + path).Data()]];
            fileAttribs = [[NSFileManager defaultManager] attributesOfItemAtPath:npath error:NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed GetFileInfo, exception:" + [[exception reason] UTF8String]);
        }
        
        NSDate *creationDate = [fileAttribs objectForKey:NSFileCreationDate];
        NSDate *modificationDate = [fileAttribs objectForKey:NSFileModificationDate];
        
        NSCalendar *gregorianCal = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];
        NSDateComponents *creationDateComps = [gregorianCal components: (NSCalendarUnitSecond | NSCalendarUnitMinute |
                                                                         NSCalendarUnitHour | NSCalendarUnitDay |
                                                                         NSCalendarUnitMonth | NSCalendarUnitYear)
                                                              fromDate: creationDate];
        
        NSDateComponents *modificationDateComps = [gregorianCal components: (NSCalendarUnitSecond | NSCalendarUnitMinute |
                                                                             NSCalendarUnitHour | NSCalendarUnitDay |
                                                                             NSCalendarUnitMonth | NSCalendarUnitYear)
                                                                  fromDate: modificationDate];
        
        res.mCreatedDate = TimeStamp((int)[creationDateComps second], (int)[creationDateComps minute], (int)[creationDateComps hour],
                                     (int)[creationDateComps day], (int)[creationDateComps month], (int)[creationDateComps year]);
        
        res.mEditDate = TimeStamp((int)[modificationDateComps second], (int)[modificationDateComps minute], (int)[modificationDateComps hour],
                                  (int)[modificationDateComps day], (int)[modificationDateComps month], (int)[modificationDateComps year]);
        
        res.mAccessDate = res.mEditDate;
        
        res.mSize = (UInt)[fileAttribs fileSize];
        
		return res;
	}

	bool FileSystem::SetFileEditDate(const String& path, const TimeStamp& time) const
	{
        bool result = false;
        @try
        {
            NSString* filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + path).Data()]];
            
            NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];
            NSDateComponents *components = [[NSDateComponents alloc] init];
            [components setYear:time.mYear];
            [components setMonth:time.mMonth];
            [components setDay:time.mDay];
            [components setHour:time.mHour];
            [components setMinute:time.mMinute];
            [components setSecond:time.mSecond];
                                    
            NSDate* date = [calendar dateFromComponents:components];
            NSDictionary* attr = [NSDictionary dictionaryWithObjectsAndKeys: date, NSFileModificationDate, NULL];
            result = [[NSFileManager defaultManager] setAttributes:attr ofItemAtPath:filePath error: NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed SetFileEditDate, exception:" + [[exception reason] UTF8String]);
        }
        
		return result;
	}

	bool FileSystem::FolderCreate(const String& path, bool recursive /*= true*/) const
	{
		if (IsFolderExist(path))
			return true;
        
        bool result = false;
        
        @try
        {
            NSString* filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + path).Data()]];
            result = [[NSFileManager defaultManager] createDirectoryAtPath:filePath withIntermediateDirectories:recursive attributes:nil error:NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed FolderCreate, exception:" + [[exception reason] UTF8String]);
        }
        
        return result;
	}

	bool FileSystem::FolderCopy(const String& from, const String& to) const
	{
		if (!IsFolderExist(from) || !IsFolderExist(to))
			return false;
        
        bool result = false;
        
        @try
        {
            NSString* sourcePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + from).Data()]];
            NSString* destPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + to).Data()]];
            
            result = [[NSFileManager defaultManager] copyItemAtPath:sourcePath toPath:destPath error:NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed FolderCopy, exception:" + [[exception reason] UTF8String]);
        }
        
        return result;
	}

	bool FileSystem::FolderRemove(const String& path, bool recursive /*= true*/) const
	{
		if (!IsFolderExist(path))
			return false;
        
        bool result = false;
        
        @try
        {
            NSString* filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + path).Data()]];
            result = [[NSFileManager defaultManager] removeItemAtPath:filePath error:NULL];
        }
        @catch (NSException *exception)
        {
            mInstance->mLog->Error((String)"Failed FileDelete, exception:" + [[exception reason] UTF8String]);
        }
        
        return result;
	}

	bool FileSystem::Rename(const String& old, const String& newPath) const
	{
		int res = rename(old, newPath);
		return res == 0;
	}

	bool FileSystem::IsFolderExist(const String& path) const
    {
        NSString* folderPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + path).Data()]];
        BOOL isDir = NO;
        BOOL isFile = [[NSFileManager defaultManager] fileExistsAtPath:folderPath isDirectory:&isDir];

		return isDir == YES && isFile == YES;
	}

	bool FileSystem::IsFileExist(const String& path) const
    {
        NSString* filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:[NSString stringWithUTF8String:("/../" + path).Data()]];
        BOOL isDir = NO;
        BOOL isFile = [[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDir];
        
        return isDir == NO && isFile == YES;
	}
}
