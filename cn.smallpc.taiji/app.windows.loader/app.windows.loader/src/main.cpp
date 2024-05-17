
#ifndef _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#endif

#ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#endif

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <filesystem>
#include <codecvt>

#include <urlmon.h>
#include <wininet.h>
#include <shellapi.h>

#include <D:/Program Files (x86)/Visual Leak Detector/include/vld.h>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

#include "../../../src/lib/windows.hpp"

using std::cin;
using std::cout;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;
using std::ifstream;
using std::getline;

namespace fs = std::filesystem;

//#define MODE_TEST

static wstring const kUpdaterVer(L"3");
static wstring const kAppVer(L"1.5.0");

#if defined _WIN64

#define PLATFORM_64

#else

#define PLATFORM_32

#endif

namespace taiji {

	static bool is64BitOS() {
		::SYSTEM_INFO info;
		::GetNativeSystemInfo(&info);
		return info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
			|| info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
	}

	static void exit(std::nullptr_t const &) {
		std::system("pause");
		std::exit(1);
		return;
	}
	static void exit(string const & msg = nullptr) {
		cout << msg << endl;
		std::system("pause");
		std::exit(1);
		return;
	}
	static void exit(wstring const & msg = nullptr) {
		wcout << msg << endl;
		std::system("pause");
		std::exit(1);
		return;
	}

	static auto getCurDrive() {
		auto s = fs::current_path().wstring();
		return wstring(s, 0, 1);
	}

	static wstring const kDataDir(L"C:\\ProgramData\\cn.smallpc\\taiji\\");

	static wstring const kLibPath(kDataDir + L"taiji.dll");

	class Lib {

		using TFTest = decltype(&::\u592A\u6975Test);
		using TFGetVerName = decltype(&\u592A\u6975GetVerName);
		using TFRunEntry = decltype(&\u592A\u6975RunEntry);
		using TFRunEntryBySpecialForWindows = decltype(&\u592A\u6975RunEntryBySpecialForWindows);

		decltype(::LoadLibraryW(nullptr)) $lib;
		TFTest $fTest;
		TFGetVerName $fGetVerName;
		TFRunEntry $fRunEntry;
		TFRunEntryBySpecialForWindows $fRunEntryBySpecialForWindows;

	public:

		// todo
		class LinkError {

			string $msg;

		public:

			string what() const {
				return "library link error : " + $msg;
			}

		};

		bool state() const {
			return $lib != nullptr;
		}

		string getVerName() const {
			if ($fGetVerName == nullptr)
				return string("");
			else
				return string($fGetVerName());
		}

		void runEntry(int const & argc, char const * const * const & argv) const {
			if ($fRunEntryBySpecialForWindows == nullptr)
				exit("can not run entry");
			$fRunEntryBySpecialForWindows(argc, argv);
			cout << endl << "finish core call" << endl << endl;
			return;
		}

		Lib() {
			$lib = ::LoadLibraryW(kLibPath.c_str());
			if (state()) {
				$fTest = (TFTest)::GetProcAddress($lib, (char *)u8"\u592A\u6975Test");
				$fGetVerName = (TFGetVerName)::GetProcAddress($lib, (char *)u8"\u592A\u6975GetVerName");
				$fRunEntry = (TFRunEntry)::GetProcAddress($lib, (char *)u8"\u592A\u6975RunEntry");
				$fRunEntryBySpecialForWindows = (TFRunEntryBySpecialForWindows)::GetProcAddress($lib, (char *)u8"\u592A\u6975RunEntryBySpecialForWindows");
				if ($fTest == nullptr) {
					exit("func not found : Test");
				}
				if ($fGetVerName == nullptr) {
					exit("func not found : GetCoreInfo");
				}
				if ($fRunEntry == nullptr) {
					exit("func not found : RunEntry");
				}
				if ($fRunEntryBySpecialForWindows == nullptr) {
					exit("func not found : RunEntryBySpecialForWindows");
				}
			} else {
				$fTest = nullptr;
				$fGetVerName = nullptr;
				$fRunEntry = nullptr;
				$fRunEntryBySpecialForWindows = nullptr;
			}
		}
		~Lib() {
			if (state()) {
				::FreeLibrary($lib);
			}
		}

	};

	namespace updater {

		class MyDownloadCallback : public ::IBindStatusCallback {

		public:

			MyDownloadCallback() {}
			virtual ~MyDownloadCallback() {}

			STDMETHOD_(ULONG, AddRef)() {
				return 0;
			}

			STDMETHOD(QueryInterface)
				(REFIID riid,
				 void __RPC_FAR * __RPC_FAR * ppvObject) {
				return E_NOTIMPL;
			}

			STDMETHOD_(ULONG, Release)() {
				return 0;
			}

			STDMETHOD(GetBindInfo)
				(DWORD __RPC_FAR * grfBINDF,
				 BINDINFO __RPC_FAR * pbindinfo) {
				return E_NOTIMPL;
			}

			STDMETHOD(GetPriority)
				(LONG __RPC_FAR * pnPriority) {
				return E_NOTIMPL;
			}

			STDMETHOD(OnDataAvailable)
				(DWORD grfBSCF,
				 DWORD dwSize,
				 FORMATETC __RPC_FAR * pformatetc,
				 STGMEDIUM __RPC_FAR * pstgmed) {
				return E_NOTIMPL;
			}

			STDMETHOD(OnLowResource)
				(DWORD reserved) {
				return E_NOTIMPL;
			}

			STDMETHOD(OnObjectAvailable)
				(REFIID riid,
				 IUnknown __RPC_FAR * punk) {
				return E_NOTIMPL;
			}

			STDMETHOD(OnProgress)
				(ULONG ulProgress,
				 ULONG ulProgressMax,
				 ULONG ulStatusCode,
				 LPCWSTR wszStatusText) {
				::printf_s("\rdownloading ... %4.1f%%         ", ulProgress * 100.0 / ulProgressMax);
				return S_OK;
			}

			STDMETHOD(OnStartBinding)
				(DWORD dwReserved,
				 IBinding __RPC_FAR * pib) {
				return E_NOTIMPL;
			}

			STDMETHOD(OnStopBinding)
				(HRESULT hresult,
				 LPCWSTR szError) {
				::printf_s("\n");
				return E_NOTIMPL;
			}

		};

		static wstring const kDownloadDirURL(
		#if defined MODE_TEST
			L"https://smallpc.cn/taiji/download-test/"
		#else
			L"https://smallpc.cn/taiji/download/"
		#endif
		);
		static wstring const kResDownloadDirURL(kDownloadDirURL + L"res/");
		static wstring const kInfoFileURL(kDownloadDirURL +
									  #if defined PLATFORM_64
										  L"update/windows.txt"
									  #else
										  L"update/windows-32.txt"
									  #endif
		);
		static wstring const kInfoFilePath(kDataDir + L"tmp/update.txt");
		static wstring const kNewLoaderFilePath(kDataDir + L"loader.exe");

		static auto makeDownloadURL(wstring const & rpath) {
			return kDownloadDirURL + rpath;
		}
		static auto makeResDownloadURL(wstring const & rpath) {
			return kResDownloadDirURL + rpath;
		}

		static auto download(wstring const & url, wstring const & dst) {
			wcout << L"download file : " << url << endl;
			fs::create_directories(fs::path(dst).parent_path());
			auto deleteState = ::DeleteUrlCacheEntryW(url.c_str());
			if (deleteState == FALSE) {
				if (::GetLastError() != ERROR_FILE_NOT_FOUND) {
					cout << "delete cache failed" << endl;
				}
			}
			MyDownloadCallback cb;
			auto result = ::URLDownloadToFileW(nullptr, url.c_str(), dst.c_str(), 0, &cb);
			if (result != S_OK) {
				exit("file download failed : " + std::to_string(result));
			}
			return;
		}

		wstring u8stows(const string & str) {
			wstring ret;
			try {
				std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
				ret = wcv.from_bytes(str);
			} catch (const std::exception & e) {
				std::cerr << e.what() << endl;
			}
			return ret;
		}

		static auto update(string const & curEXEPath) {
			cout << "if you want to update or install TaiJi, please press any key, else you should exit me." << endl;
			std::system("pause");
			fs::create_directories(kDataDir);
			cout << "download updater-info ..." << endl;
			download(kInfoFileURL, kInfoFilePath);
			ifstream f(kInfoFilePath);
			if (!f.is_open()) {
				cout << "[updater-error] open update-info file failed" << endl;
				return;
			}
			f.seekg(0);
			{
				string updaterVer;
				getline(f, updaterVer);
				if (u8stows(updaterVer) != L"windows-updater-version=" + kUpdaterVer) {
					// 信息文件版本不匹配，需要用户手动获取app更新
					cout << "[updater-error] updater version too low , please update from website" << endl;
					fs::remove(kInfoFilePath);
					return;
				}
			}
			{
				string appVer, appURL;
				getline(f, appVer);
				getline(f, appURL);
				if (u8stows(appVer) != kAppVer) {
					// 进行app更新
					cout << "found new version 'loader', updating ..." << endl;
					fs::remove(kInfoFilePath);
					// 检查当前运行的exe是否占用指定下载路径
					if (u8stows(curEXEPath) == kNewLoaderFilePath) {
						wcout << (L"[updater-error] need to update 'loader' app, but the file already exists, please rename then restart : " + kNewLoaderFilePath) << endl;
						return;
					}
					// 下载更新，并打开所在文件夹，退出程序
					download(makeResDownloadURL(u8stows(appURL)), kNewLoaderFilePath);
					::ShellExecuteW(nullptr, L"explore", kDataDir.c_str(), nullptr, nullptr, SW_SHOW);
					wcout << (L"latest version's loader is downloaded to path : " + kNewLoaderFilePath + L"\nplease delete me.") << endl;
					return;
				}
			}
			{
				string libVer, libURL;
				getline(f, libVer);
				getline(f, libURL);
				if (libVer != Lib().getVerName()) {
					// 进行lib更新
					cout << "found new version 'core', updating ..." << endl;
					download(makeResDownloadURL(u8stows(libURL)), kLibPath);
					cout << "core update finish" << endl;
				}
			}
			{
				cout << "download dependencies files ..." << endl;
				while (!f.eof()) {
					string path;
					wstring wpath, dst;
					getline(f, path);
					wpath = u8stows(path);
					dst = kDataDir + wpath;
					download(makeResDownloadURL(wpath), dst);
				}
			}
			f.close();
			fs::remove(kInfoFilePath);
			cout << "update finish" << endl;
			return;
		}

	}

}

int main(int argc, char * argv[]) {
	::SetConsoleCP(CP_UTF8);
	::SetConsoleOutputCP(CP_UTF8);

	wcout << L"TaiJi-Loader " << kAppVer << endl;

	cout << "your device is " << (taiji::is64BitOS() ? "64-bit" : "32-bit") << endl;

	if (argc == 1) {
		taiji::updater::update(argv[0]);
	} else {
		taiji::Lib().runEntry(argc - 1, argv + 1);
	}

	std::system("pause");
	return 0;
}
