#include "SIPModel.h"
#include "pjsip/sip_msg.h"
#include "SIPPresenter.h"
#include <fstream>
#include <iostream>
#include <sstream>
bool VoIPhone::SIPModel::initPJ()
{
    pj::EpConfig ep_cfg;
    pj::TransportConfig tcfg;
    tcfg.port = 5060;
    try {
        core.libCreate();
        core.libInit(ep_cfg);
        core.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
        core.libStart();
		core.libRegisterThread("modelThread");
    }
    catch (pj::Error& err) {
		System::Console::WriteLine("Инициализация библиотеки завершилась с ошибкой \r\n Error #" + gcnew System::String(err.info().data()));
        return false;
    }
	std::ifstream ifs("D:\\PSU\\DIPLOM\\Debug\\KEY.txt");
	//std::ifstream ifs("C:\\Users\\sdxop\\Desktop\\share\\DIPLOM\\Debug\\KEY.txt");
	if (!ifs)
		std::cerr << "mda";
	ifs >> key;
	if (key != "")
		cryptFlag = 1;
	ifs.close();
    return true;
}
bool VoIPhone::SIPModel::destroyPJ()
{
	try {
		core.hangupAllCalls();
		if (acc != nullptr) {
			acc->shutdown();
			delete acc;
			
		}
		if (this->exitAccount()) {
			core.libStopWorkerThreads();
			core.libDestroy();
		}
	}
	catch (pj::Error& err) {
		System::Console::WriteLine("Деинициализация библиотеки завершилась с ошибкой \r\n Error #" + gcnew System::String(err.info().data()));
		return false;
	}
	return true;
} 
bool VoIPhone::SIPModel::loginAccount(System::Collections::ArrayList^ destination, SIPPresenter^ pres)
{
	//acc = new SIPaccount();
	acc = new SIPaccount(pres);
	System::Collections::Generic::List<System::String^>^ userData = gcnew System::Collections::Generic::List<System::String^>();
	for each (auto item in destination)
	{
		userData->Add(dynamic_cast<System::String^>(item));
	}
	
	uData userDataNString;
	userDataNString.sipServ		= (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(userData[0])).ToPointer();
	userDataNString.sipProxy	= (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(userData[1])).ToPointer();
	userDataNString.sipDomain	= (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(userData[2])).ToPointer();
	userDataNString.sipUsername = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(userData[3])).ToPointer();
	userDataNString.sipPassword = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(userData[4])).ToPointer();
	
	pj::AccountConfig acc_cfg;
	acc_cfg.idUri					  = "sip:" + userDataNString.sipUsername + "@" + userDataNString.sipServ;
	acc_cfg.regConfig.registrarUri    = "sip:" + userDataNString.sipServ;
	acc_cfg.sipConfig.proxies.push_back("sip:" + userDataNString.sipProxy);
	acc_cfg.sipConfig.authCreds.push_back(pj::AuthCredInfo("Digets", "*", userDataNString.sipUsername, PJSIP_CRED_DATA_PLAIN_PASSWD, userDataNString.sipPassword));
	try {
		acc->create(acc_cfg, true);
		this->buddyCreate();
	}
	catch (pj::Error& err) {
		System::Console::WriteLine("Авторизация аккаунта завершилась с ошибкой(Create) \r\n Error #" + gcnew System::String(err.info().data()));
		return false;
	}
	if (!acc->isValid()) {
		System::Console::WriteLine("Error in valid account");
		return false;
	}
	else {
		try {
			pj::PresenceStatus ps;
			ps.status = PJSUA_BUDDY_STATUS_ONLINE;
			// Optional, set the activity and some note
			ps.activity = PJRPID_ACTIVITY_BUSY;
			ps.note = "On the phone";
			acc->setOnlineStatus(ps);
		}
		catch (pj::Error& err) {
			System::Console::WriteLine("Инициализация библиотеки завершилась с ошибкой(PresenceStatus) \r\n Error #" + gcnew System::String(err.info().data()));
			return false;
		}
	}
	return true;
}
bool VoIPhone::SIPModel::buddyCreate()
{
	buddy = new MyBuddy;
	pj::BuddyConfig bdd_cfg;
	pj::AccountInfo acc_inf;
	acc_inf = acc->getInfo();
	bdd_cfg.uri = acc_inf.uri;
	try {
		buddy->create(*acc, bdd_cfg);
		buddy->subscribePresence(true);
	}
	catch (pj::Error& err) {
		System::Windows::Forms::MessageBox::Show("Инициализация библиотеки завершилась с ошибкой(byddyCreate) \r\n Error #" + gcnew System::String(err.info().data()));
		return false;
	}
	return true;
}
bool VoIPhone::SIPModel::exitAccount()
{
	try {
		if (acc != NULL) {
			acc->shutdown();
		}
	}
	catch (pj::Error& err) {
		System::Windows::Forms::MessageBox::Show("Выход из аккаунта завершился с ошибкой \r\n Error #" + gcnew System::String(err.info().data()));
		return false;
	}
	return true;
}
void VoIPhone::SIPModel::makeCallSs(System::String^ destination)
{
	int startIndex = destination->IndexOf("\r\n");
	String^ resultString = destination->Substring(startIndex + 2);
	const char* chAddress = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(resultString)).ToPointer();
	std::string userName = chAddress;
	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chAddress));
	
	if (key == "key-magmakey-enc") {
		MyCall* myCall = new MyCall(*acc);
		pj::Call* call = myCall;
		acc->calls.push_back(call);
		pj::CallOpParam prm(true);
		prm.opt.audioCount = 1;
		prm.opt.videoCount = 0;
		myCall->encr = 1;
		std::thread callTh([this, userName, call, prm] {
			CallThread(userName, call, prm);
			});
		callTh.detach();
	}
	else {
		pj::Call* call = new MyCall(*acc);
		acc->calls.push_back(call);
		pj::CallOpParam prm(true);
		prm.opt.audioCount = 1;
		prm.opt.videoCount = 0;

		std::thread callTh([this, userName, call, prm] {
			CallThread(userName, call, prm);
			});
		callTh.detach();
	}
}
void VoIPhone::SIPModel::makeCallNs(System::String^ destination)
{
	if (this->copyFlag == 1) {
		std::string fileName = "asdf.txt";
		std::ofstream fs("D:\\PSU\\DIPLOM\\Debug\\" + fileName);
		//std::ofstream fs("C:\\Users\\sdxop\\Desktop\\share\\DIPLOM\\Debug\\" + fileName);
		if (!fs) {
			std::cout << "mda";
		}
		if (this->cryptFlag = 1) {
			fs << "Привет, я переданный файл!\nПересылаю тебе!";
			//fs << "ІГ`ѓЪoбйKЖZjЧyбW‹_©b{ ™ЊҐeЯ - ";
		}
		else
			fs << "Привет, я переданный файл!";
		fs.close();
		this->copyFlag = 0;
	}
	else{
		int startIndex = destination->IndexOf("\r\n");
		String^ resultString = destination->Substring(startIndex + 2);
		const char* chAddress = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(resultString)).ToPointer();
		std::string userName = chAddress;
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chAddress));

		std::string fileName = "asdf.txt";
		std::ofstream fs("D:\\PSU\\DIPLOM\\Debug\\" + fileName);
		//std::ofstream fs("C:\\Users\\sdxop\\Desktop\\share\\DIPLOM\\Debug\\" + fileName);
		if (!fs) {
			std::cout << "mda";
		}
		fs << "Привет, я переданный файл!";
		fs.close();

		std::string file;
		System::Windows::Forms::OpenFileDialog^ openDlg = gcnew System::Windows::Forms::OpenFileDialog();
		openDlg->Title = "Выбор файла для отправки";
		openDlg->Filter = "Text Files(*.TXT;*.DOC;*.DOCX)|*.TXT;*.DOC;*.DOCX|All files (*.*)|*.*";
		if (System::Windows::Forms::DialogResult::OK == openDlg->ShowDialog())
		{
			System::String^ fileName = openDlg->FileName;
			int startIndex = fileName->IndexOf("\r\n");
			System::String^ resultString = fileName->Substring(startIndex + 2);
			const char* chAddress = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(resultString)).ToPointer();
			file = chAddress;
			System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)chAddress));
		}

		if (this->cryptFlag == 1) {
			this->copyFlag = 1;
			acc->cryptFlag = 1;
			int         startIndex = destination->IndexOf("\r\n");
			String^ resultString = destination->Substring(startIndex + 2);
			const char* chAddress = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(resultString)).ToPointer();
			std::string msg = "ІГ`ѓЪoбйKЖZjЧyбW‹_©b{ ™ЊҐeЯ -";
			std::string userName = chAddress;
			pj::BuddyConfig cfg;
			cfg.uri = userName;
			pj::Buddy* buddy = new pj::Buddy();
			buddy->create(*acc, cfg);
			pj::SendInstantMessageParam simp;
			simp.content = msg;
			System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chAddress));
			try {
				buddy->sendInstantMessage(simp);
			}
			catch (pj::Error& err) {
				std::cout << err.info() << std::endl;
			}
			delete buddy;
		}
		else {
			this->copyFlag = 1;
			int         startIndex = destination->IndexOf("\r\n");
			String^ resultString = destination->Substring(startIndex + 2);
			const char* chAddress = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(resultString)).ToPointer();
			std::string msg = "Файл - " + fileName;
			std::string userName = chAddress;
			pj::BuddyConfig cfg;
			cfg.uri = userName;
			pj::Buddy* buddy = new pj::Buddy();
			buddy->create(*acc, cfg);
			pj::SendInstantMessageParam simp;
			simp.content = msg;
			System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chAddress));
			try {
				buddy->sendInstantMessage(simp);
			}
			catch (pj::Error& err) {
				std::cout << err.info() << std::endl;
			}
			delete buddy;
		}
	}
}
void VoIPhone::SIPModel::CallThread(std::string callAddress, pj::Call* call, pj::CallOpParam prm)
{
	pj_thread_desc desc;
	pj_thread_t* thread = 0;
	pj_thread_register("callIs", desc, &thread);
	
	try {
		call->makeCall(callAddress, prm);
	}
	catch (pj::Error& err) {
		std::cout << "Error with call " + err.info() << std::endl;
		acc->calls.pop_back();
		return;
	}
	_callActive = true;
}
void VoIPhone::SIPModel::hangupCalls()
{
	try {
		for (size_t i = 0; i < acc->calls.size(); ++i)
			acc->calls.pop_back();
	}
	catch(...){}
	core.hangupAllCalls();
	_callActive = false;
}
void VoIPhone::SIPModel::sendMessage(System::String^ msg, System::String^ fromUri) const
{
	if (this->cryptFlag == 1) {
		int         startIndex = fromUri->IndexOf("\r\n");
		String^ resultString = fromUri->Substring(startIndex + 2);
		const char* chAddress = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(resultString)).ToPointer();
		//const char* chAddress     = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(fromUri)).ToPointer();
		const char* chMsg = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(msg)).ToPointer();
		std::string userName = chAddress;
		pj::BuddyConfig cfg;
		cfg.uri = userName;
		pj::Buddy* buddy = new pj::Buddy();
		acc->cryptFlag = 1;
		buddy->create(*acc, cfg);
		pj::SendInstantMessageParam simp;
		std::string strMsg = chMsg;
		std::string strMsgE = "";
		Magma magma(key.c_str(), "mouse");
		std::istringstream strStream(strMsg);
		std::ostringstream strStreamE(strMsgE);
		magma.encrypt(Magma::Method::CFB, strStream, strStreamE);
		simp.content = strStreamE.str();
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chAddress));
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chMsg));
		try {
			buddy->sendInstantMessage(simp);
		}
		catch (pj::Error& err) {
			std::cout << err.info() << std::endl;
		}
		delete buddy;
	}
	else {
		int         startIndex = fromUri->IndexOf("\r\n");
		String^ resultString = fromUri->Substring(startIndex + 2);
		const char* chAddress = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(resultString)).ToPointer();
		//const char* chAddress     = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(fromUri)).ToPointer();
		const char* chMsg = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(msg)).ToPointer();
		std::string userName = chAddress;
		pj::BuddyConfig cfg;
		cfg.uri = userName;
		pj::Buddy* buddy = new pj::Buddy();
		buddy->create(*acc, cfg);
		pj::SendInstantMessageParam simp;
		simp.content = chMsg;
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chAddress));
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)chMsg));
		try {
			buddy->sendInstantMessage(simp);

		}
		catch (pj::Error& err) {
			std::cout << err.info() << std::endl;
		}
		delete buddy;
	}
}
System::String^ VoIPhone::SIPModel::addContact()
{
	AddContactForm^ addContact = gcnew AddContactForm();
	addContact->ShowDialog();
	System::Collections::Generic::List<System::String^>^ contactInfo = addContact->getInfo();
	if (contactInfo != nullptr) {
		//_friendsManager->addFriend(contactInfo[0], contactInfo[1]);
		System::String^ req = contactInfo[0] + "-" + contactInfo[1];
		//System::Windows::Forms::MessageBox::Show("Адреса пользователя не существует");
		return req;
	}
	return nullptr;
}