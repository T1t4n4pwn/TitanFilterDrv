#include "TitanGui.h"
#include <iostream>
#include <vector>
#include <Windows.h>

#include "imgui/imgui.h"

#include "Utils.h"

std::vector<TableRuleData> tableDataList;

static void DrawRuleTable() {

    if (ImGui::BeginTable("RuleTable", 6, ImGuiTableFlags_Borders)) {

        ImGui::TableSetupColumn(u8"选择");
        ImGui::TableSetupColumn(u8"进程名");
        ImGui::TableSetupColumn(u8"文件路径");
        ImGui::TableSetupColumn(u8"路径类型");
        ImGui::TableSetupColumn(u8"拒绝访问");
        ImGui::TableSetupColumn(u8"隐藏");

        ImGui::TableHeadersRow();


        for (int i = 0; i < tableDataList.size(); i++) {

            TableRuleData& data = tableDataList[i];
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);

            ImGui::PushID(i);
            ImGui::Checkbox("Check", data.getCheckedPtr());
            ImGui::PopID();

            ImGui::TableSetColumnIndex(1);
            ImGui::Text(data.getProcessName().c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text(data.getFilePath().c_str());


            std::string strType;
            if (data.getPathType() == FLT_PATH_TYPE_EQUAL) {
                strType = "Equal";
            }
            else if (data.getPathType() == FLT_PATH_TYPE_INCLUDE) {
                strType = "Include";
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::Text(strType.c_str());

            ImGui::TableSetColumnIndex(4);
            ImGui::Text(data.isDenyAccess() ? "Y" : "N");

            ImGui::TableSetColumnIndex(5);
            ImGui::Text(data.isHideFile() ? "Y" : "N");
        }



        ImGui::EndTable();
    }
}

static void BtnDelSelectRuleEvent() {
    
    std::vector<TableRuleData>::iterator it;
    TableRuleData theData;

    MiniFilterCommunication comm;
    if (!comm.Connect(MINIFILTER_PORT_NAME)) {
        MessageBox(0, L"连接服务失败", L"Error", MB_ICONERROR);
        return;
    }

    for (size_t i = 0; i < tableDataList.size(); i++)
    {
        TableRuleData crtData = tableDataList[i];
        if (!crtData.getChecked()) {
            continue;
        }

        DataPacket packet;
        packet.setType(PACKET_DEL_PATH);
        packet.setProcessName(String2Wstring(crtData.getProcessName()));
        packet.setFilePath(String2Wstring(crtData.getFilePath()));
        if (!comm.SendMessageToFlt(packet)) {
            MessageBox(0, L"与服务通信失败", L"Error", MB_ICONERROR);
            return;
        }

    }

    tableDataList.erase(std::remove_if(tableDataList.begin(), tableDataList.end(), [&](TableRuleData& e) {
        return e.getChecked();
        }), tableDataList.end());

}

static void BtnStartEvent() {

    MiniFilterCommunication comm;
    if (!comm.Connect(MINIFILTER_PORT_NAME)) {
        MessageBox(0,L"连接服务失败", L"Error", MB_ICONERROR);
        return;
    }

    DataPacket packet;
    packet.setType(PACKET_START);
    if (!comm.SendMessageToFlt(packet)) {
        MessageBox(0, L"与服务通信失败", L"Error", MB_ICONERROR);
        return;
    }

}

static void BtnStopEvent() {
    
    MiniFilterCommunication comm;
    if (!comm.Connect(MINIFILTER_PORT_NAME)) {
        MessageBox(0, L"连接服务失败", L"Error", MB_ICONERROR);
        return;
    }

    DataPacket packet;
    packet.setType(PACKET_STOP);
    if (!comm.SendMessageToFlt(packet)) {
        MessageBox(0, L"与服务通信失败", L"Error", MB_ICONERROR);
        return;
    }

}

void DrawCallBack() {

    ImGui::SetNextWindowPos({ 0, 0 });

    ImGui::Begin("Hello, world!", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    ImVec2 btnSize = ImVec2{ 80, 40 };
    ImGui::SetNextWindowPos({0, 0});
    
    
    ImGui::BeginTabBar("#TabBar123");
    if (ImGui::BeginTabItem("Main")) {
        
        if (ImGui::Button(u8"开始", btnSize)) {
            BtnStartEvent();
        }
        ImGui::SameLine();

        if (ImGui::Button(u8"删除选中", btnSize)) {
            BtnDelSelectRuleEvent();
        }
        ImGui::SameLine();

        if (ImGui::Button(u8"停止", btnSize)) {
            BtnStopEvent();
        }

        ImGui::Separator();
        ImGui::NewLine();

        DrawRuleTable();

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("AddRule")) {

        static char processName[260]{0};
        static char filePath[260]{0};
        static bool isDenyAccess = false;
        static bool isHideFile = false;

        ImGui::InputText("ProcessName", processName, sizeof(processName));

        ImGui::InputText("FilePath", filePath, sizeof(filePath));

        ImGui::Checkbox("DenyAccess", &isDenyAccess);
        ImGui::Checkbox("HideFile", &isHideFile);

        const char* descript[] = {"Equal", "Include"};
        static int select = 0;
        ImGui::Combo("Combo", &select, descript, sizeof(descript) / sizeof(char*));

        if(ImGui::Button("Add", btnSize)) {

            bool allowAdd = true;

            if (strlen(processName) == 0 || strlen(filePath) == 0) {
                MessageBox(0, L"进程名或文件路径不能为空", L"Error", 0);
                allowAdd = false;
            }

            if (!isDenyAccess && !isHideFile) {
                MessageBox(0, L"拒绝访问与隐藏至少选则一个", L"Error", 0);
                allowAdd = false;
            }

            if (allowAdd) {

                TableRuleData data;

                data.setPathType((FLT_PATH_TYPE)select);
                data.setProcessName(processName);
                data.setFilePath(filePath);
                data.setDenyAccess(isDenyAccess);
                data.setHideFile(isHideFile);

                tableDataList.push_back(data);

                ULONG protectFlags = 0;

                if (isDenyAccess) {
                    protectFlags |= FLT_RULE_FILE_DENY_ACESS;
                }
                if (isHideFile) {
                    protectFlags |= FLT_RULE_FILE_HIDE;
                }

                MiniFilterCommunication comm;
                if (!comm.Connect(MINIFILTER_PORT_NAME)) {
                    MessageBox(0, L"连接服务失败", L"Error", MB_ICONERROR);
                    return;
                }

                DataPacket packet;
                packet.setType(PACKET_ADD_PATH);
                packet.setProcessName(String2Wstring(data.getProcessName()));
                packet.setFilePath(String2Wstring(data.getFilePath()));
                packet.setProtectFlags(protectFlags);
                if (!comm.SendMessageToFlt(packet)) {
                    MessageBox(0, L"与服务通信失败", L"Error", MB_ICONERROR);
                    return;
                }

            }
            

        }

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Other")) {

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    

    

    ImGui::End();
}