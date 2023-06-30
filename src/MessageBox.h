#pragma once

#include <functional>
#include <future>
#include <vector>

// Thanks to skyrim scripting
namespace MES {

    class SkyrimMessageBox {
        class MessageBoxResultCallback : public RE::IMessageBoxCallback {
            std::function<void(unsigned int)> _callback;

        public:
            ~MessageBoxResultCallback() override {}
            MessageBoxResultCallback(std::function<void(unsigned int)> callback) : _callback(callback) {}
            void Run(RE::IMessageBoxCallback::Message message) override {
                _callback(static_cast<unsigned int>(message));
            }
        };

    public:
        static void Show(const std::string& bodyText, std::vector<std::string> buttonTextValues,
            std::function<void(unsigned int)> callback) {
            auto* factoryManager = RE::MessageDataFactoryManager::GetSingleton();
            auto* uiStringHolder = RE::InterfaceStrings::GetSingleton();
            auto* factory = factoryManager->GetCreator<RE::MessageBoxData>(
                uiStringHolder->messageBoxData);  // "MessageBoxData" <--- can we just use this string?
            auto* messagebox = factory->Create();
            RE::BSTSmartPointer<RE::IMessageBoxCallback> messageCallback =
                RE::make_smart<MessageBoxResultCallback>(callback);
            messagebox->callback = messageCallback;
            messagebox->bodyText = bodyText;
            for (auto text : buttonTextValues) messagebox->buttonText.push_back(text.c_str());
            messagebox->QueueMessage();
        }

        static void Hide()
        {
            if (RE::UI::GetSingleton()->GetMenu(RE::MessageBoxMenu::MENU_NAME))
            {
                RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
                msgQueue->AddMessage(RE::MessageBoxMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kForceHide, nullptr);
            };
        }
    };

    void ShowMessageBox(const std::string& bodyText, std::vector<std::string> buttonTextValues,
        std::function<void(unsigned int)> callback) {
        SkyrimMessageBox::Show(bodyText, buttonTextValues, callback);
    }
}