#include "SIPPresenter.h"
#include "MainForm.h"
#include "IncomigCallForm.h"
#include "SIPModel.h"
#include "SIPaccount.h"
VoIPhone::SIPPresenter::SIPPresenter(MainForm^ v) {
	model = new SIPModel;
	view = v;
	if (model->initPJ()) {
		view->sendFile += gcnew System::Action(this, &SIPPresenter::OnSendFileButtonClicked);
		view->sendMsg += gcnew System::Action(this, &SIPPresenter::OnSendMessageButtonClicked);
		view->makeCall += gcnew System::Action(this, &SIPPresenter::OnMakeCallButtonClicked);
		view->updateSelectContact += gcnew System::Action(this, &SIPPresenter::OnUpdateSelectedContact);
		view->addContactButtonClicked += gcnew System::Action(this, &SIPPresenter::OnAddContactButtonClicked);
		view->destroyPJ += gcnew System::Action(this, &SIPPresenter::destroyLib);
		view->LoginRegButtonClicked += gcnew System::Action(this, &SIPPresenter::OnLoginRegButtonClicked);
		view->ExitAccButtonClicked += gcnew System::Action(this, &SIPPresenter::OnExitButtonClicked);
	}
	else
		System::Windows::Forms::MessageBox::Show("Ошибка инициализации PJSIP");
}
void VoIPhone::SIPPresenter::OnLoginRegButtonClicked() {
	if (view->_accRegFlag == ACC_NOT_REGISTERED) {
		RegistrationForm^ regForm = gcnew RegistrationForm();
		regForm->ShowDialog();
		ArrayList^ accountInfo = regForm->getData();
		if (model->loginAccount(accountInfo, this)) {
			view->loginRegButton->Text = L"Выйти";
			view->loginAccountInfo->Text = L"SIP ID: " + accountInfo[3]->ToString() + L"@" + accountInfo[2]->ToString();
			view->_accRegFlag = ACC_IS_REGISTERED;
		}
		else
			System::Windows::Forms::MessageBox::Show("Введен неправильный логин или пароль!");
	}
	else if (view->_accRegFlag == ACC_IS_REGISTERED) {
		if (MessageBox::Show(
			L"Вы действительно хотите выйти?",
			L"Выход",
			MessageBoxButtons::YesNo,
			MessageBoxIcon::Exclamation
		) == System::Windows::Forms::DialogResult::Yes) {
			OnExitButtonClicked();
		}
	}
}
void VoIPhone::SIPPresenter::OnExitButtonClicked() {
	if (model->exitAccount()) {
		view->loginRegButton->Text = L"Войти";
		view->loginAccountInfo->Text = L"Войдите в SIP";
		view->_accRegFlag = ACC_NOT_REGISTERED;
		view->contactsListBox->Items->Clear();
		view->contactConInfoLabel->Text = "";
	}
}
void VoIPhone::SIPPresenter::OnAddContactButtonClicked()
{
	if (view->_accRegFlag == ACC_IS_REGISTERED) {
		view->contactsListBox->Items->Add(model->addContact());
	}
	else if (view->_accRegFlag == ACC_NOT_REGISTERED) {
		System::Windows::Forms::MessageBox::Show("Для начала войдите в аккаунт");
	}
}
void VoIPhone::SIPPresenter::destroyLib()
{
	if (!this->_destroy)
		model->destroyPJ();
	this->_destroy = 1;
}
void VoIPhone::SIPPresenter::OnUpdateSelectedContact()
{
	if (view->contactsListBox->SelectedItem != nullptr) {
		view->contactConInfoLabel->Text = "";
		auto tx = view->contactsListBox->SelectedItem->ToString();
		array<System::String^>^ parts = tx->Split('-');
		if (parts[0] == "")
			parts[0] = "def";
		for each (String ^ %part in parts) {
			part = part->Substring(0, 1) + part->Substring(1);
		}
		System::String^ correctedString = System::String::Join("\r\n", parts);
		view->contactConInfoLabel->Text = correctedString;
		view->chatRichTextBox->Clear();
	}
}
void VoIPhone::SIPPresenter::OnMakeCallButtonClicked()
{
	if (view->_accRegFlag == ACC_IS_REGISTERED) {
		if (view->_inActiveCall == ACC_CALL_NO_ACTIVE) {
			if (view->contactConInfoLabel->Text != "") {
				model->makeCallSs(view->contactConInfoLabel->Text->ToString());
				view->_inActiveCall = ACC_CALL_ACTIVE;
				view->makeCallButton->Text = "Завершить";
			}
			else
				System::Windows::Forms::MessageBox::Show("Вы не выбрали кому звонить");
		}
		else if (view->_inActiveCall == ACC_CALL_ACTIVE) {
			if (view->chatRichTextBox->Text != "") {
				//view->chatRichTextBox->SelectedText = view->chatRichTextBox->Text + System::Environment::NewLine + "Соединение разорвано...";
			}
			model->hangupCalls();
			view->_inActiveCall = ACC_CALL_NO_ACTIVE;
			view->makeCallButton->Text = "Вызов";
		}
	}
	else if (view->_accRegFlag == ACC_NOT_REGISTERED)
		System::Windows::Forms::MessageBox::Show("Для начала войдите в аккаунт");
}
void VoIPhone::SIPPresenter::OnSendMessageButtonClicked()
{
	if (view->_accRegFlag == ACC_IS_REGISTERED) {
		model->sendMessage(view->sendMessageTexBox->Text->ToString(), view->contactConInfoLabel->Text->ToString());
		view->sendMessageTexBox->Clear();
	}
	else if (view->_accRegFlag == ACC_NOT_REGISTERED) {
		MessageBox::Show("Для начала войдите в аккаунт");
	}
}
void VoIPhone::SIPPresenter::OnSendFileButtonClicked()
{
	if (view->_accRegFlag == ACC_IS_REGISTERED) {

		if (view->contactConInfoLabel->Text != "") {
			model->makeCallNs(view->contactConInfoLabel->Text->ToString());
		}
		else
			System::Windows::Forms::MessageBox::Show("Вы не выбрали кому отправлять файл");
		if (view->chatRichTextBox->Text != "") {
			//view->chatRichTextBox->SelectedText = view->chatRichTextBox->Text + System::Environment::NewLine + "Соединение разорвано...";
		}
	}
	else if (view->_accRegFlag == ACC_NOT_REGISTERED)
		System::Windows::Forms::MessageBox::Show("Для начала войдите в аккаунт");
}
void VoIPhone::SIPPresenter::updateChatMsgTextBox(System::String^ url, System::String^ msg)
{
	incMsg = gcnew array<Object^> { url, msg };
	view->chatRichTextBox->Invoke(gcnew MethodInvoker(this, &SIPPresenter::UpdateChatHelper), incMsg);
}
void VoIPhone::SIPPresenter::UpdateChatHelper()
{
	array<Object^>^ args = dynamic_cast<array<Object^>^>(view->chatRichTextBox->Invoke(gcnew Func<array<Object^>^>(this, &SIPPresenter::GetArgs)));
	updateChat(args);
}
array<System::Object^>^ VoIPhone::SIPPresenter::GetArgs()
{
	return incMsg;
}
void VoIPhone::SIPPresenter::updateChat(array<Object^>^ args)
{
	System::String^ url = safe_cast<System::String^>(args[0]);
	System::String^ msg = safe_cast<System::String^>(args[1]);

	if (view->chatRichTextBox->Text == "") {
		view->chatRichTextBox->SelectedText = url + "-> " + msg;
	}
	else {
		view->chatRichTextBox->SelectedText = System::Environment::NewLine;
		view->chatRichTextBox->SelectedText = url + "-> " + msg;
	}
}
void VoIPhone::SIPPresenter::answerCall(System::String^ url)
{
	incCall = gcnew array<Object^> {url};
	view->Invoke(gcnew MethodInvoker(this, &SIPPresenter::updateCall));
}
void VoIPhone::SIPPresenter::updateCall()
{
	System::String^ url = safe_cast<System::String^>(incCall[0]);
	int index = url->IndexOf('<');
	System::String^ resultUrl;
	System::String^ name;
	System::String^ address;
	if (index != -1)
	{
		if (index != 0)
			name = url->Substring(1, index - 3)->Trim();
		address = url->Substring(index + 1, url->Length - index - 2)->Trim();
		resultUrl = name + "\r\n" + address;
	}
	if (view->_inActiveCall == ACC_CALL_NO_ACTIVE) {
		view->_inActiveCall = ACC_CALL_ACTIVE;
		//view->contactConInfoLabel->Text = resultUrl;
		//view->contactsListBox->Items->Add(name + "-" + address);
		//view->chatRichTextBox->Clear();
		//view->chatRichTextBox->Text = "Соединение установленно... " + address;
		//view->makeCallButton->Text = "Завершить";
	}
	else if (view->_inActiveCall == ACC_CALL_ACTIVE) {
		model->hangupCalls();
		view->_inActiveCall = ACC_CALL_ACTIVE;
		//view->contactConInfoLabel->Text = resultUrl;
		//view->contactsListBox->Items->Add(name + "-" + address);
		//view->chatRichTextBox->Clear();
		//view->makeCallButton->Text = "Завершить";
	}
}

