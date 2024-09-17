#pragma once
#ifndef __MAINFORM_H
#define __MAINFORM_H
#include "SIPPresenter.h"
namespace VoIPhone {
	/// <summary>
	/// Сводка для MainForm
	/// </summary>
	enum StatusAcc {
		ACC_IS_REGISTERED,
		ACC_NOT_REGISTERED,
		ACC_CALL_ACTIVE,
		ACC_CALL_NO_ACTIVE
	};
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			gcnew SIPPresenter(this);
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
			destroyPJ();
		}
	public:
		System::Windows::Forms::ContextMenuStrip^ contextMenuStrip1;
		System::Windows::Forms::SplitContainer^ mainSplitContainer;
		System::Windows::Forms::Button^ loginRegButton;
		System::Windows::Forms::Label^ loginAccountInfo;
		System::Windows::Forms::Button^ addContactButton;
		System::Windows::Forms::ListBox^ contactsListBox;
		System::ComponentModel::IContainer^ components;
		System::Windows::Forms::RichTextBox^ chatRichTextBox;
		System::Windows::Forms::Label^ contactConInfoLabel;
		System::Windows::Forms::Button^ sendTextButton;
		System::Windows::Forms::Button^ sendFileButton;
		System::Windows::Forms::TextBox^ sendMessageTexBox;
		System::Windows::Forms::Button^ makeCallButton;

		int _inActiveCall = ACC_CALL_NO_ACTIVE;
		int _accRegFlag = ACC_NOT_REGISTERED;
		//Вызов входа в аккаунт
		event System::Action^ LoginRegButtonClicked;
		//Вызов выхода из аккаунта
		event System::Action^ ExitAccButtonClicked;
		//Вызов закрытия всех компонентов
		event System::Action^ destroyPJ;
		//Вызов добавления в контакты
		event System::Action^ addContactButtonClicked;
		//Вызов обновления выбранного контакта
		event System::Action^ updateSelectContact;
		//Вызов функции звонка
		event System::Action^ makeCall;
		//Отправка сообщений
		event System::Action^ sendMsg;
		//Отправка файла
		event System::Action^ sendFile;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->mainSplitContainer = (gcnew System::Windows::Forms::SplitContainer());
			this->addContactButton = (gcnew System::Windows::Forms::Button());
			this->contactsListBox = (gcnew System::Windows::Forms::ListBox());
			this->loginRegButton = (gcnew System::Windows::Forms::Button());
			this->loginAccountInfo = (gcnew System::Windows::Forms::Label());
			this->sendTextButton = (gcnew System::Windows::Forms::Button());
			this->sendFileButton = (gcnew System::Windows::Forms::Button());
			this->sendMessageTexBox = (gcnew System::Windows::Forms::TextBox());
			this->makeCallButton = (gcnew System::Windows::Forms::Button());
			this->contactConInfoLabel = (gcnew System::Windows::Forms::Label());
			this->chatRichTextBox = (gcnew System::Windows::Forms::RichTextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->mainSplitContainer))->BeginInit();
			this->mainSplitContainer->Panel1->SuspendLayout();
			this->mainSplitContainer->Panel2->SuspendLayout();
			this->mainSplitContainer->SuspendLayout();
			this->SuspendLayout();
			// 
			// mainSplitContainer
			// 
			this->mainSplitContainer->Dock = System::Windows::Forms::DockStyle::Fill;
			this->mainSplitContainer->IsSplitterFixed = true;
			this->mainSplitContainer->Location = System::Drawing::Point(0, 0);
			this->mainSplitContainer->Name = L"mainSplitContainer";
			// 
			// mainSplitContainer.Panel1
			// 
			this->mainSplitContainer->Panel1->Controls->Add(this->addContactButton);
			this->mainSplitContainer->Panel1->Controls->Add(this->contactsListBox);
			this->mainSplitContainer->Panel1->Controls->Add(this->loginRegButton);
			this->mainSplitContainer->Panel1->Controls->Add(this->loginAccountInfo);
			// 
			// mainSplitContainer.Panel2
			// 
			this->mainSplitContainer->Panel2->Controls->Add(this->sendTextButton);
			this->mainSplitContainer->Panel2->Controls->Add(this->sendFileButton);
			this->mainSplitContainer->Panel2->Controls->Add(this->sendMessageTexBox);
			this->mainSplitContainer->Panel2->Controls->Add(this->makeCallButton);
			this->mainSplitContainer->Panel2->Controls->Add(this->contactConInfoLabel);
			this->mainSplitContainer->Panel2->Controls->Add(this->chatRichTextBox);
			this->mainSplitContainer->Size = System::Drawing::Size(784, 561);
			this->mainSplitContainer->SplitterDistance = 261;
			this->mainSplitContainer->TabIndex = 0;
			// 
			// addContactButton
			// 
			this->addContactButton->BackColor = System::Drawing::Color::Gray;
			this->addContactButton->FlatAppearance->BorderSize = 0;
			this->addContactButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->addContactButton->Font = (gcnew System::Drawing::Font(L"Arial", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->addContactButton->ForeColor = System::Drawing::SystemColors::Info;
			this->addContactButton->Location = System::Drawing::Point(7, 13);
			this->addContactButton->Name = L"addContactButton";
			this->addContactButton->Size = System::Drawing::Size(248, 50);
			this->addContactButton->TabIndex = 3;
			this->addContactButton->Text = L"Добавить контакт";
			this->addContactButton->UseVisualStyleBackColor = false;
			this->addContactButton->Click += gcnew System::EventHandler(this, &MainForm::addContactButton_Click);
			// 
			// contactsListBox
			// 
			this->contactsListBox->BackColor = System::Drawing::SystemColors::WindowFrame;
			this->contactsListBox->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->contactsListBox->ForeColor = System::Drawing::Color::Silver;
			this->contactsListBox->FormattingEnabled = true;
			this->contactsListBox->ItemHeight = 27;
			this->contactsListBox->Location = System::Drawing::Point(7, 69);
			this->contactsListBox->Name = L"contactsListBox";
			this->contactsListBox->Size = System::Drawing::Size(248, 382);
			this->contactsListBox->TabIndex = 2;
			this->contactsListBox->DoubleClick += gcnew System::EventHandler(this, &MainForm::contactsListBox_DoubleClick);
			// 
			// loginRegButton
			// 
			this->loginRegButton->BackColor = System::Drawing::Color::Gray;
			this->loginRegButton->FlatAppearance->BorderSize = 0;
			this->loginRegButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->loginRegButton->Font = (gcnew System::Drawing::Font(L"Arial", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->loginRegButton->ForeColor = System::Drawing::SystemColors::Info;
			this->loginRegButton->Location = System::Drawing::Point(3, 514);
			this->loginRegButton->Name = L"loginRegButton";
			this->loginRegButton->Size = System::Drawing::Size(252, 39);
			this->loginRegButton->TabIndex = 1;
			this->loginRegButton->Text = L"Войти";
			this->loginRegButton->UseVisualStyleBackColor = false;
			this->loginRegButton->Click += gcnew System::EventHandler(this, &MainForm::loginRegButton_Click);
			// 
			// loginAccountInfo
			// 
			this->loginAccountInfo->Font = (gcnew System::Drawing::Font(L"Arial", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->loginAccountInfo->ForeColor = System::Drawing::SystemColors::Info;
			this->loginAccountInfo->Location = System::Drawing::Point(3, 465);
			this->loginAccountInfo->Name = L"loginAccountInfo";
			this->loginAccountInfo->Size = System::Drawing::Size(252, 46);
			this->loginAccountInfo->TabIndex = 0;
			this->loginAccountInfo->Text = L"Войдите в SIP ";
			this->loginAccountInfo->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// sendTextButton
			// 
			this->sendTextButton->Location = System::Drawing::Point(429, 514);
			this->sendTextButton->Name = L"sendTextButton";
			this->sendTextButton->Size = System::Drawing::Size(78, 35);
			this->sendTextButton->TabIndex = 5;
			this->sendTextButton->Text = L"Отправить";
			this->sendTextButton->UseVisualStyleBackColor = true;
			this->sendTextButton->Click += gcnew System::EventHandler(this, &MainForm::sendTextButton_Click);
			// 
			// sendFileButton
			// 
			this->sendFileButton->Location = System::Drawing::Point(358, 514);
			this->sendFileButton->Name = L"sendFileButton";
			this->sendFileButton->Size = System::Drawing::Size(64, 35);
			this->sendFileButton->TabIndex = 4;
			this->sendFileButton->Text = L"Файл";
			this->sendFileButton->UseVisualStyleBackColor = true;
			this->sendFileButton->Click += gcnew System::EventHandler(this, &MainForm::sendFileButton_Click);
			// 
			// sendMessageTexBox
			// 
			this->sendMessageTexBox->Location = System::Drawing::Point(3, 514);
			this->sendMessageTexBox->Multiline = true;
			this->sendMessageTexBox->Name = L"sendMessageTexBox";
			this->sendMessageTexBox->Size = System::Drawing::Size(349, 35);
			this->sendMessageTexBox->TabIndex = 3;
			// 
			// makeCallButton
			// 
			this->makeCallButton->BackColor = System::Drawing::Color::Gray;
			this->makeCallButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->makeCallButton->ForeColor = System::Drawing::SystemColors::Info;
			this->makeCallButton->Location = System::Drawing::Point(412, 13);
			this->makeCallButton->Name = L"makeCallButton";
			this->makeCallButton->Size = System::Drawing::Size(95, 50);
			this->makeCallButton->TabIndex = 2;
			this->makeCallButton->Text = L"Вызов";
			this->makeCallButton->UseVisualStyleBackColor = false;
			this->makeCallButton->Click += gcnew System::EventHandler(this, &MainForm::makeCallButton_Click);
			// 
			// contactConInfoLabel
			// 
			this->contactConInfoLabel->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->contactConInfoLabel->Location = System::Drawing::Point(87, 13);
			this->contactConInfoLabel->Name = L"contactConInfoLabel";
			this->contactConInfoLabel->Size = System::Drawing::Size(306, 50);
			this->contactConInfoLabel->TabIndex = 1;
			// 
			// chatRichTextBox
			// 
			this->chatRichTextBox->Font = (gcnew System::Drawing::Font(L"Arial", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->chatRichTextBox->Location = System::Drawing::Point(3, 69);
			this->chatRichTextBox->Name = L"chatRichTextBox";
			this->chatRichTextBox->Size = System::Drawing::Size(504, 422);
			this->chatRichTextBox->TabIndex = 0;
			this->chatRichTextBox->Text = L"";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::WindowFrame;
			this->ClientSize = System::Drawing::Size(784, 561);
			this->Controls->Add(this->mainSplitContainer);
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"VoiPhone v0.1";
			this->mainSplitContainer->Panel1->ResumeLayout(false);
			this->mainSplitContainer->Panel2->ResumeLayout(false);
			this->mainSplitContainer->Panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->mainSplitContainer))->EndInit();
			this->mainSplitContainer->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
#pragma region FormsButtons
	private:
		System::Void loginRegButton_Click(System::Object^ sender, System::EventArgs^ e) {
			LoginRegButtonClicked();
		}
		//Кнопка добавления контакта
		System::Void addContactButton_Click(System::Object^ sender, System::EventArgs^ e) {
			addContactButtonClicked();
		}
		//Кнопка обновления информации о контактке
		System::Void contactsListBox_DoubleClick(System::Object^ sender, System::EventArgs^ e) {
			updateSelectContact();
		}
		//Кнопка вызова контакта
		System::Void makeCallButton_Click(System::Object^ sender, System::EventArgs^ e) {
			makeCall();
		}
		//Кнопка отправки сообщения
		System::Void sendTextButton_Click(System::Object^ sender, System::EventArgs^ e) {
			sendMsg();
		}
		 System::Void sendFileButton_Click(System::Object^ sender, System::EventArgs^ e) {
			sendFile();
		}
};
#pragma endregion
}
#endif // !__MAINFORM_H
 

//Добавить обработку конца вызоваю
//Чтобы кнопка изменила свойство текст