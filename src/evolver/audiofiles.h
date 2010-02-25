/*	Copyright � 2007 Apple Inc. All Rights Reserved.
	
	Disclaimer: IMPORTANT:  This Apple software is supplied to you by 
			Apple Inc. ("Apple") in consideration of your agreement to the
			following terms, and your use, installation, modification or
			redistribution of this Apple software constitutes acceptance of these
			terms.  If you do not agree with these terms, please do not use,
			install, modify or redistribute this Apple software.
			
			In consideration of your agreement to abide by the following terms, and
			subject to these terms, Apple grants you a personal, non-exclusive
			license, under Apple's copyrights in this original Apple software (the
			"Apple Software"), to use, reproduce, modify and redistribute the Apple
			Software, with or without modifications, in source and/or binary forms;
			provided that if you redistribute the Apple Software in its entirety and
			without modifications, you must retain this notice and the following
			text and disclaimers in all such redistributions of the Apple Software. 
			Neither the name, trademarks, service marks or logos of Apple Inc. 
			may be used to endorse or promote products derived from the Apple
			Software without specific prior written permission from Apple.  Except
			as expressly stated in this notice, no other rights or licenses, express
			or implied, are granted by Apple herein, including but not limited to
			any patent rights that may be infringed by your derivative works or by
			other works in which the Apple Software may be incorporated.
			
			The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
			MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
			THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
			FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
			OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
			
			IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
			OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
			SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
			INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
			MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
			AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
			STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
			POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef NYNEX_AUDIOFILES
#define NYNEX_AUDIOFILES

/*
	Compile options:
		CAAUDIOFILE_PROFILE
*/

#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <AudioToolbox/AudioToolbox.h>
#else
	#include <AudioToolbox.h>
#endif
#include "CAStreamBasicDescription.h"
#include "CABufferList.h"
#include "CAHostTimeBase.h"
#include "CAAudioChannelLayout.h"
#include "CAExtAudioFile.h"

#if TARGET_OS_WIN32
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#endif

#define kMaxUserProps 8
namespace nynex {
class FileReaderWriter {
public:
	// options for ConversionParameters.flags
	enum {
		kOpt_OverwriteOutputFile	= 1,
		kOpt_Verbose				= 2,
		kOpt_CAFTag					= 4,				// tags encoded CAF files with info about the source file
														// or restores based on it when decoding
		kOpt_NoSanitizeOutputFormat = 8					// used internally
	};
	
	struct UserProperty {
		UInt32	propertyID;		// four char code of the property
		SInt32	propertyValue;	// value of the property 
	};
    
    struct ReadBuffer {
        ReadBuffer() : readBuffer(NULL), readPtrs(NULL), nFrames(0) {}
        ~ReadBuffer() { 
            if (readPtrs != NULL) { delete readPtrs; } 
            if (readBuffer != NULL) { delete readBuffer; }
        }
        
        CABufferList *readBuffer;
        CABufferList *readPtrs;
        UInt32 nFrames;
    };
	
	struct ConversionParameters {
		ConversionParameters();
		
		UInt32					flags;
		
		struct {
            const char *            filePath;
			AudioFileID				audioFileID;		// or set this to non-zero
			AudioChannelLayoutTag	channelLayoutTag;	// 0 to use the one in the file, or default
			UInt32					trackIndex;
		}						input;
		
		struct {
            const char *            filePath;
			AudioFileTypeID			fileType;
			CAStreamBasicDescription dataFormat;
			int						channels;			// -1 for same number of channels as input
			SInt32					bitRate;			// -1 for default
			SInt32					codecQuality;		// 0-127, -1 for default
			SInt32					srcQuality;			// 0-127, -1 for default
			UInt32					srcComplexity;		// 'norm' or 'bats'
			SInt32					strategy;			// 0-2, -1 for default
			SInt32					primeMethod;		// 0-2, -1 for default
			AudioChannelLayoutTag	channelLayoutTag;	// 0 for default
			UserProperty			userProperty[kMaxUserProps];	// arbitrary properties and values
			UInt32					creationFlags;		// flags for AudioFileCreate
		}						output;
		
		//UInt32					bufferSize;				// [UNIMPLEMENTED]
	};
	
	FileReaderWriter();
	virtual ~FileReaderWriter();
    
    void            Prepare(const ConversionParameters &);
	void			Input(const char * filename);
	void			Output(const char * filename);
    ReadBuffer *    GetNextReadBuffer() throw (CAXException);
    void            WriteFromBuffer(ReadBuffer *) throw (CAXException);
    
	virtual bool	Progress(SInt64 packetsConverted, SInt64 packetsTotal) { return false; }
	virtual void	GenerateOutputFileName(const char *inputFilePath, 
						const CAStreamBasicDescription &inputFormat,
						const CAStreamBasicDescription &outputFormat,
						OSType outputFileType, char *outName);
	
	const char *	GetOutputFileName() { return mOutName; }
	const CAExtAudioFile &	InputFile() { return mSrcFile; }
	const CAExtAudioFile & OutputFile() { return mDestFile; }
	
	// customization
	virtual void	OpenInputFile();
	virtual void	OpenOutputFile(const CAStreamBasicDescription &destFormat, CAAudioChannelLayout &destFileLayout);
	virtual void	BeginConversion() { }
	virtual void	EndConversion() { }
	virtual bool	ShouldTerminateConversion() { return false; }

	bool	TaggedEncodingToCAF() const {
				return (mParams.flags & kOpt_CAFTag) && mDestFormat.mFormatID != kAudioFormatLinearPCM && mParams.output.fileType == kAudioFileCAFType;
			}
	bool	TaggedDecodingFromCAF() const {
				return (mParams.flags & kOpt_CAFTag) && mSrcFormat.mFormatID != kAudioFormatLinearPCM;
					// don't know for sure that the source file is CAF...
			}
	
	void	WriteCAFInfo();
	void	ReadCAFInfo();
    void                        CloseInputFile();
    void                        CloseOutputFile();
	
protected:
    void                        PrepareConverters();
    void                        CloseFiles() { CloseInputFile(); CloseOutputFile(); }
	ConversionParameters        mParams;
    bool                        mOpenedSourceFile;
    bool                        mCreatedOutputFile;
    bool                        mEncoding;
    bool                        mDecoding;
    bool                        mReady;
	CAExtAudioFile				mSrcFile;
	CAExtAudioFile				mDestFile;
	CAStreamBasicDescription	mSrcFormat;		// valid after OpenInputFile
	CAStreamBasicDescription	mDestFormat;	// valid after OpenOutputFile
	CAStreamBasicDescription	mSrcClientFormat;		// valid after OpenInputFile
	CAStreamBasicDescription	mDestClientFormat;	// valid after OpenOutputFile
    CAAudioChannelLayout        mSrcFileLayout, mDstFileLayout;
    CAAudioChannelLayout        mSrcClientLayout, mDstClientLayout;

protected:
	char						mOutName[PATH_MAX];
};
}
#endif // NYNEX_AUDIOFILES
