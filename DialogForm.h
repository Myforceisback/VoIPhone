#pragma once
#include "SIPModel.h"
#include <regex>
#include <sstream>
#include "SIPaccount.h"


namespace VoIPhone {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	enum StatusAcc {
		ACC_IS_REGISTERED,
		ACC_NOT_REGISTERED,
		ACC_CALL_ACTIVE,
		ACC_CALL_NO_ACTIVE,
		ACC_NO_FIRST_ANSWER,
		ACC_FIRST_ANSWER
	};
	/// <summary>
	/// Сводка для DialogForm
	/// </summary>
	public ref class DialogForm : public System::Windows::Forms::Form
	{
	public:
		DialogForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}
		//Вызывается при входящем вызове
		DialogForm(SIPaccount& acc, pj::CallInfo* ci, pj::Call* call, pj::CallOpParam* prm)
		{
			this->acc = &acc;
			model = new SIPModel(this->acc);
			this->ci = ci;
			this->call = call;
			this->prm = prm;
			_isActiveCall = ACC_CALL_ACTIVE;
			_answrCall = ACC_FIRST_ANSWER;
			InitializeComponent();
			model->buddyCreate();
			this->contactInfoLabel->Text = gcnew String(ci->remoteUri.c_str());
			this->callButton->Text = L"Сбросить";
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~DialogForm()
		{
			if (components)
			{
				delete components;
			}
			try {
				delete model;
				//delete call;
				//delete ci;
				//delete prm;
				//call->hangup(&this->prm);
			}
			catch (...) {}
		}
	private: System::Windows::Forms::Label^ contactInfoLabel;
	private: System::Windows::Forms::TextBox^ textSenderTextBox;
	private: System::Windows::Forms::Button^ callButton;
	public: System::Windows::Forms::RichTextBox^ chatRichTextBox;



	protected:

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;
	private:
		SIPaccount* acc;
		pj::CallInfo* ci;
		pj::Call* call;
		pj::CallOpParam* prm;
		VoIPhone::callData* callD;
		SIPModel* model;
		int _isActiveCall;
		int _answrCall;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->contactInfoLabel = (gcnew System::Windows::Forms::Label());
			this->textSenderTextBox = (gcnew System::Windows::Forms::TextBox());
			this->callButton = (gcnew System::Windows::Forms::Button());
			this->chatRichTextBox = (gcnew System::Windows::Forms::RichTextBox());
			this->SuspendLayout();
			// 
			// contactInfoLabel
			// 
			this->contactInfoLabel->BackColor = System::Drawing::SystemColors::AppWorkspace;
			this->contactInfoLabel->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->contactInfoLabel->Location = System::Drawing::Point(12, 9);
			this->contactInfoLabel->Name = L"contactInfoLabel";
			this->contactInfoLabel->Size = System::Drawing::Size(271, 50);
			this->contactInfoLabel->TabIndex = 0;
			this->contactInfoLabel->Text = L"MASHA\r\nSIP:masha@sip2sip.ru";
			// 
			// textSenderTextBox
			// 
			this->textSenderTextBox->BackColor = System::Drawing::SystemColors::ScrollBar;
			this->textSenderTextBox->Location = System::Drawing::Point(12, 355);
			this->textSenderTextBox->MaximumSize = System::Drawing::Size(531, 55);
			this->textSenderTextBox->MinimumSize = System::Drawing::Size(531, 55);
			this->textSenderTextBox->Name = L"textSenderTextBox";
			this->textSenderTextBox->Size = System::Drawing::Size(531, 55);
			this->textSenderTextBox->TabIndex = 1;
			this->textSenderTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &DialogForm::textSenderTextBox_KeyPress);
			// 
			// callButton
			// 
			this->callButton->Location = System::Drawing::Point(466, 9);
			this->callButton->Name = L"callButton";
			this->callButton->Size = System::Drawing::Size(77, 50);
			this->callButton->TabIndex = 2;
			this->callButton->Text = L"Позвонить";
			this->callButton->UseVisualStyleBackColor = true;
			this->callButton->Click += gcnew System::EventHandler(this, &DialogForm::callButton_Click);
			// 
			// chatRichTextBox
			// 
			this->chatRichTextBox->Location = System::Drawing::Point(12, 65);
			this->chatRichTextBox->Name = L"chatRichTextBox";
			this->chatRichTextBox->ReadOnly = true;
			this->chatRichTextBox->Size = System::Drawing::Size(531, 284);
			this->chatRichTextBox->TabIndex = 3;
			this->chatRichTextBox->Text = L"";
			// 
			// DialogForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::WindowFrame;
			this->ClientSize = System::Drawing::Size(555, 420);
			this->Controls->Add(this->chatRichTextBox);
			this->Controls->Add(this->callButton);
			this->Controls->Add(this->textSenderTextBox);
			this->Controls->Add(this->contactInfoLabel);
			this->Name = L"DialogForm";
			this->Text = L"Чат";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

public:
	void UpdateRichTextBox(std::string msg)
	{
		System::Windows::Forms::MessageBox::Show(gcnew System::String(msg.c_str()));
	}

private: System::Void callButton_Click(System::Object^ sender, System::EventArgs^ e) {
		/*if (_isActiveCall == ACC_CALL_ACTIVE) {
			if (_answrCall == ACC_FIRST_ANSWER) {
				acc->calls.pop_back();
				call->hangup(&this->prm);
			}
			else if (_answrCall == ACC_NO_FIRST_ANSWER) {
				call->hangup(&this->prm);
			}
			this->callButton->Text = L"Вызов";
			_isActiveCall = ACC_CALL_NO_ACTIVE;
		}
		else if (_isActiveCall == ACC_CALL_NO_ACTIVE) {
			this->callButton->Text = L"Сбросить";
			_isActiveCall = ACC_CALL_ACTIVE;
			callD = (VoIPhone::callData*)&model->makeCallNs(this->ci->remoteUri);
			call = callD->call;
			prm = callD->prm;
		}*/
	}
private: System::Void textSenderTextBox_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
	using namespace System;
	using namespace System::Text::RegularExpressions;
	if (e->KeyChar == (char)Keys::Enter) {

		std::regex pattern("<([^>]*)>");
		std::smatch match;
		std::string extractedString;
		if (std::regex_search(this->ci->remoteUri, match, pattern)) {
			extractedString = match[1];
		}
		else {
			std::cout << "Не удалось извлечь строку" << std::endl;
		}

		model->sendMessage(this->textSenderTextBox->Text->ToString(), gcnew String(extractedString.c_str()));
		if (this->chatRichTextBox->Text == "") {
			this->chatRichTextBox->SelectedText = L"ME -> " + this->textSenderTextBox->Text->ToString();
			this->textSenderTextBox->Clear();
		}
		else {
			this->chatRichTextBox->SelectedText = System::Environment::NewLine;
			this->chatRichTextBox->SelectedText = L"ME -> " + this->textSenderTextBox->Text->ToString();
			this->textSenderTextBox->Clear();
		}
		
	}
}
};
}
