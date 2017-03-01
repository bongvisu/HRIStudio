// This is a generated source file for Chilkat version 9.5.0.56
#ifndef _C_CkStreamWH
#define _C_CkStreamWH
#include "chilkatDefs.h"

#include "Chilkat_C.h"


CK_VISIBLE_PUBLIC void CkStreamW_setAbortCheck(HCkStreamW cHandle, BOOL (*fnAbortCheck)());
CK_VISIBLE_PUBLIC void CkStreamW_setPercentDone(HCkStreamW cHandle, BOOL (*fnPercentDone)(int pctDone));
CK_VISIBLE_PUBLIC void CkStreamW_setProgressInfo(HCkStreamW cHandle, void (*fnProgressInfo)(const wchar_t *name, const wchar_t *value));
CK_VISIBLE_PUBLIC void CkStreamW_setTaskCompleted(HCkStreamW cHandle, void (*fnTaskCompleted)(HCkTaskW hTask));

CK_VISIBLE_PUBLIC HCkStreamW CkStreamW_Create(void);
CK_VISIBLE_PUBLIC HCkStreamW CkStreamW_Create2(BOOL bCallbackOwned);
CK_VISIBLE_PUBLIC void CkStreamW_Dispose(HCkStreamW handle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getCanRead(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getCanWrite(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getDataAvailable(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_getDebugLogFilePath(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void CkStreamW_putDebugLogFilePath(HCkStreamW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_debugLogFilePath(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC int CkStreamW_getDefaultChunkSize(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_putDefaultChunkSize(HCkStreamW cHandle, int newVal);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getEndOfStream(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getIsWriteClosed(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_getLastErrorHtml(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_lastErrorHtml(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_getLastErrorText(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_lastErrorText(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_getLastErrorXml(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_lastErrorXml(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getLastMethodSuccess(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_putLastMethodSuccess(HCkStreamW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC __int64 CkStreamW_getLength(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC __int64 CkStreamW_getNumReceived(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC __int64 CkStreamW_getNumSent(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC int CkStreamW_getReadFailReason(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC int CkStreamW_getReadTimeoutMs(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_putReadTimeoutMs(HCkStreamW cHandle, int newVal);
CK_VISIBLE_PUBLIC void CkStreamW_getSinkFile(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void CkStreamW_putSinkFile(HCkStreamW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_sinkFile(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_getSourceFile(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void CkStreamW_putSourceFile(HCkStreamW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_sourceFile(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getStringBom(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_putStringBom(HCkStreamW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC void CkStreamW_getStringCharset(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC void CkStreamW_putStringCharset(HCkStreamW cHandle, const wchar_t *newVal);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_stringCharset(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_getVerboseLogging(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_putVerboseLogging(HCkStreamW cHandle, BOOL newVal);
CK_VISIBLE_PUBLIC void CkStreamW_getVersion(HCkStreamW cHandle, HCkString retval);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_version(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC int CkStreamW_getWriteFailReason(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC int CkStreamW_getWriteTimeoutMs(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC void CkStreamW_putWriteTimeoutMs(HCkStreamW cHandle, int newVal);
CK_VISIBLE_PUBLIC BOOL CkStreamW_ReadBytes(HCkStreamW cHandle, HCkByteData outBytes);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_ReadBytesAsync(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_ReadBytesENC(HCkStreamW cHandle, const wchar_t *encoding, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_readBytesENC(HCkStreamW cHandle, const wchar_t *encoding);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_ReadBytesENCAsync(HCkStreamW cHandle, const wchar_t *encoding);
CK_VISIBLE_PUBLIC BOOL CkStreamW_ReadNBytes(HCkStreamW cHandle, int numBytes, HCkByteData outBytes);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_ReadNBytesAsync(HCkStreamW cHandle, int numBytes);
CK_VISIBLE_PUBLIC BOOL CkStreamW_ReadNBytesENC(HCkStreamW cHandle, int numBytes, const wchar_t *encoding, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_readNBytesENC(HCkStreamW cHandle, int numBytes, const wchar_t *encoding);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_ReadNBytesENCAsync(HCkStreamW cHandle, int numBytes, const wchar_t *encoding);
CK_VISIBLE_PUBLIC BOOL CkStreamW_ReadString(HCkStreamW cHandle, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_readString(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_ReadStringAsync(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_ReadToCRLF(HCkStreamW cHandle, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_readToCRLF(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_ReadToCRLFAsync(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_ReadUntilMatch(HCkStreamW cHandle, const wchar_t *matchStr, HCkString outStr);
CK_VISIBLE_PUBLIC const wchar_t *CkStreamW_readUntilMatch(HCkStreamW cHandle, const wchar_t *matchStr);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_ReadUntilMatchAsync(HCkStreamW cHandle, const wchar_t *matchStr);
CK_VISIBLE_PUBLIC void CkStreamW_Reset(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_RunStream(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_RunStreamAsync(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_SaveLastError(HCkStreamW cHandle, const wchar_t *path);
CK_VISIBLE_PUBLIC BOOL CkStreamW_SetSinkStream(HCkStreamW cHandle, HCkStreamW strm);
CK_VISIBLE_PUBLIC BOOL CkStreamW_SetSourceBytes(HCkStreamW cHandle, HCkByteData sourceData);
CK_VISIBLE_PUBLIC BOOL CkStreamW_SetSourceStream(HCkStreamW cHandle, HCkStreamW strm);
CK_VISIBLE_PUBLIC BOOL CkStreamW_SetSourceString(HCkStreamW cHandle, const wchar_t *srcStr, const wchar_t *charset);
CK_VISIBLE_PUBLIC BOOL CkStreamW_WriteByte(HCkStreamW cHandle, int byteVal);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_WriteByteAsync(HCkStreamW cHandle, int byteVal);
CK_VISIBLE_PUBLIC BOOL CkStreamW_WriteBytes(HCkStreamW cHandle, HCkByteData byteData);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_WriteBytesAsync(HCkStreamW cHandle, HCkByteData byteData);
CK_VISIBLE_PUBLIC BOOL CkStreamW_WriteBytesENC(HCkStreamW cHandle, const wchar_t *byteData, const wchar_t *encoding);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_WriteBytesENCAsync(HCkStreamW cHandle, const wchar_t *byteData, const wchar_t *encoding);
CK_VISIBLE_PUBLIC BOOL CkStreamW_WriteClose(HCkStreamW cHandle);
CK_VISIBLE_PUBLIC BOOL CkStreamW_WriteString(HCkStreamW cHandle, const wchar_t *str);
CK_VISIBLE_PUBLIC HCkTaskW CkStreamW_WriteStringAsync(HCkStreamW cHandle, const wchar_t *str);
#endif
