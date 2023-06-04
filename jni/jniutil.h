#pragma once

#include "main.h"
#include <jni.h>
#include <string>

class CJavaWrapper
{
public:
    CJavaWrapper(JNIEnv *p, jobject activity);
    ~CJavaWrapper() {};

    JNIEnv* p;

    // Фулл экран
    void SetUseFullScreen(int b);
    // Показать настройки клиента (жаба)
    void ShowClientSettings();
    // показать клавиатуру андроид
    void ShowInputLayout();
    // скрыть клавиатуру андроид
    void HideInputLayout(); 
    // получить данные (текст) с буфера обмена
    const char* GetClipboardString(); 
    // спидометр (жаба)
    void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
    void ShowSpeed();
    // on / off показ спидометра
    void HideSpeed();
    // диалоги с BR
    void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText);
    // уведомления BR
    void ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn);


    jobject activity;
    jmethodID s_SetUseFullScreen;
    jmethodID s_ShowClientSettings;
    jmethodID s_ShowInputLayout;
    jmethodID s_HideInputLayout;
    jmethodID s_GetKeyboardString;

    jmethodID s_MakeDialog; 
    jmethodID s_showNotification;
    jmethodID s_showSpeed;
    jmethodID s_hideSpeed;
    jmethodID s_updateSpeedInfo;
};