#include "CoreException.h"

CoreException::CoreException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber):
    ErrorCode(hr),
    FunctionName(functionName),
    LineNumber(lineNumber),
    FileName(fileName)
{
}

std::wstring CoreException::ProcessMessage() const
{
    _com_error error(ErrorCode);

    const char* custom_log_name = "MyLogName";

    // create registry keys for ACLing described on MSDN: http://msdn2.microsoft.com/en-us/library/aa363648.aspx


    const std::wstring customMessage =
        L"FUNCTION NAME     : " + FunctionName + L"\n" +
        L"FILE NAME         : " + FileName + L"\n" +
        L"ERROR NUMBER LINE : " + std::to_wstring(LineNumber) + L"\n" +
        L"MESSAGE           : " + error.ErrorMessage() + L"\n";

    HANDLE event_log = RegisterEventSource(NULL, L"MONKEY_SYSTEM_LOG");
    LPCWSTR message = customMessage.c_str();
    ReportEvent(event_log, EVENTLOG_SUCCESS, 0, 0, NULL, 1, 0, &message, NULL);
    return customMessage;
}
