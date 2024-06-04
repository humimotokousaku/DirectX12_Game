#include "ImGuiManager.h"
#include "SrvManager.h"

void ImGuiManager::Initialize(HWND hwnd) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(DirectXCommon::GetInstance()->GetDevice().Get(),
		DirectXCommon::GetInstance()->GetSwapChainDesc().BufferCount,
		DirectXCommon::GetInstance()->GetRtvDesc().Format,
		SrvManager::GetInstance()->GetDescriptorHeap().Get(),
		SrvManager::GetInstance()->GetDescriptorHeap().Get()->GetCPUDescriptorHandleForHeapStart(),
		SrvManager::GetInstance()->GetDescriptorHeap().Get()->GetGPUDescriptorHandleForHeapStart());
	SrvManager::GetInstance()->Allocate();
}

void ImGuiManager::PreDraw() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
}

void ImGuiManager::PostDraw() {
	ImGui::Render();
	// 実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->GetCommandList().Get());
}

void ImGuiManager::Finalize() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext(); // ImGuiのコンテキストを破棄
}