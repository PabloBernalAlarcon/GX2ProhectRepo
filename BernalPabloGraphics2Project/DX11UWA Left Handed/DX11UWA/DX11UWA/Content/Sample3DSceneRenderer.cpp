
#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace DX11UWA;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	memset(m_kbuttons, 0, sizeof(m_kbuttons));
	m_currMousePos = nullptr;
	m_prevMousePos = nullptr;
	memset(&m_camera, 0, sizeof(XMFLOAT4X4));

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources(void)
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	m_viewportOne = new D3D11_VIEWPORT();
	m_viewportOne->Height = outputSize.Height;
	m_viewportOne->Width = outputSize.Width/2;
	m_viewportOne->MinDepth = 0.0f;
	m_viewportOne->MaxDepth = 0.1f;
	m_viewportOne->TopLeftX = 0.0f;
	m_viewportOne->TopLeftY = 0.0f;

	m_viewportTwo = new D3D11_VIEWPORT();
	m_viewportTwo->Height = outputSize.Height;
	m_viewportTwo->Width = outputSize.Width/2;
	m_viewportTwo->MinDepth = 0.0f;
	m_viewportTwo->MaxDepth = 0.1f;
	m_viewportTwo->TopLeftY = 0.0;
	m_viewportTwo->TopLeftX = outputSize.Width/2;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.
	
	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	XMStoreFloat4x4(&m_ShrekconstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	XMStoreFloat4x4(&m_PercyconstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	XMStoreFloat4x4(&m_JynxconstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	XMStoreFloat4x4(&m_RTCconstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	XMStoreFloat4x4(&m_SkyconstantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 3.0f, -15.0f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 2.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_camera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	XMStoreFloat4x4(&m_ShrekconstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	XMStoreFloat4x4(&m_PercyconstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	XMStoreFloat4x4(&m_JynxconstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	XMStoreFloat4x4(&m_RTCconstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	XMStoreFloat4x4(&m_SkyconstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
		m_constantBufferData.light.x = radians;
		m_constantBufferData.light.y = radians;
		m_constantBufferData.light.z = radians;

		//set all lights to the constant buffer light
		m_ShrekconstantBufferData.light = m_constantBufferData.light;
		m_PercyconstantBufferData.light = m_constantBufferData.light;
		m_JynxconstantBufferData.light = m_constantBufferData.light;
		m_RTCconstantBufferData.light = m_constantBufferData.light;
		m_SkyconstantBufferData.light = m_constantBufferData.light;
		Rotate(radians);
	}
	

	// Update or move camera here
	UpdateCamera(timer, 5.0f, 0.75f);

	XMFLOAT4X4 yee(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	m_constantBufferData.model._14 = 0.0f;
	m_constantBufferData.model._24 = 6.0f;
	m_constantBufferData.model._34 = -8.0f;
	//m_ShrekconstantBufferData.model = yee;
	XMStoreFloat4x4(&m_ShrekconstantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(3.14f)));
	m_ShrekconstantBufferData.projection = m_constantBufferData.projection;
	m_ShrekconstantBufferData.view = m_constantBufferData.view;

	m_PercyconstantBufferData.model._24 = 6.5f;
	m_PercyconstantBufferData.model._34 = 2.0f;

	//m_PercyconstantBufferData.model = yee;
	m_PercyconstantBufferData.projection = m_constantBufferData.projection;
	m_PercyconstantBufferData.view = m_constantBufferData.view;

	m_PercyconstantBufferData.model._34 = 1.0f;
	m_JynxconstantBufferData.projection = m_constantBufferData.projection;
	m_JynxconstantBufferData.view = m_constantBufferData.view;

	m_RTCconstantBufferData.model = yee;
	m_RTCconstantBufferData.projection = m_constantBufferData.projection;
	m_RTCconstantBufferData.view = m_constantBufferData.view;

	m_SkyconstantBufferData.model = yee;
	m_SkyconstantBufferData.projection = m_constantBufferData.projection;
	m_SkyconstantBufferData.view = m_constantBufferData.view;
}

#pragma region Movements
// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	//XMMATRIX whatever = XMLoadFloat4x4(&m_ShrekconstantBufferData.model);
	//XMStoreFloat4x4(&m_ShrekconstantBufferData.model, XMMatrixMultiply(XMMatrixIdentity(),XMMatrixTranspose(XMMatrixRotationY(radians))));
	XMStoreFloat4x4(&m_ShrekconstantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(-radians)));

	XMStoreFloat4x4(&m_RTCconstantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(-radians)));

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationX(radians)));

	XMStoreFloat4x4(&m_JynxconstantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(-radians)));

	XMMATRIX modelPos = XMMatrixTranspose(XMMatrixTranslation(5, 0, 0));
	XMStoreFloat4x4(&m_PercyconstantBufferData.model, XMMatrixMultiply(modelPos, XMMatrixTranspose(XMMatrixRotationY(radians*10.0f))));
	XMMATRIX modelWorld = XMLoadFloat4x4(&m_PercyconstantBufferData.model);
	XMStoreFloat4x4(&m_PercyconstantBufferData.model, XMMatrixMultiply(XMMatrixTranspose(XMMatrixRotationY(radians)), modelWorld));
	

}

void Sample3DSceneRenderer::UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd)
{
	const float delta_time = (float)timer.GetElapsedSeconds();

	if (m_kbuttons['W'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
		
	}
	if (m_kbuttons['S'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['A'])
	{
		XMMATRIX translation = XMMatrixTranslation(-moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['D'])
	{
		XMMATRIX translation = XMMatrixTranslation(moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['X'])
	{
		XMMATRIX translation = XMMatrixTranslation( 0.0f, -moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons[VK_SPACE])
	{
		XMMATRIX translation = XMMatrixTranslation( 0.0f, moveSpd * delta_time, 0.0f*10);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}

	if (m_currMousePos) 
	{
		if (m_currMousePos->Properties->IsRightButtonPressed && m_prevMousePos)
		{
			float dx = m_currMousePos->Position.X - m_prevMousePos->Position.X;
			float dy = m_currMousePos->Position.Y - m_prevMousePos->Position.Y;

			XMFLOAT4 pos = XMFLOAT4(m_camera._41, m_camera._42, m_camera._43, m_camera._44);

			m_camera._41 = 0;
			m_camera._42 = 0;
			m_camera._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotSpd * delta_time);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotSpd * delta_time);

			XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
			temp_camera = XMMatrixMultiply(rotX, temp_camera);
			temp_camera = XMMatrixMultiply(temp_camera, rotY);

			XMStoreFloat4x4(&m_camera, temp_camera);

			m_camera._41 = pos.x;
			m_camera._42 = pos.y;
			m_camera._43 = pos.z;
		}
		m_prevMousePos = m_currMousePos;
	}


}

void Sample3DSceneRenderer::SetKeyboardButtons(const char* list)
{
	memcpy_s(m_kbuttons, sizeof(m_kbuttons), list, sizeof(m_kbuttons));
}

void Sample3DSceneRenderer::SetMousePosition(const Windows::UI::Input::PointerPoint^ pos)
{
	m_currMousePos = const_cast<Windows::UI::Input::PointerPoint^>(pos);
}

#pragma endregion

void Sample3DSceneRenderer::SetInputDeviceData(const char* kb, const Windows::UI::Input::PointerPoint^ pos)
{
	SetKeyboardButtons(kb);
	SetMousePosition(pos);
}

void DX11UWA::Sample3DSceneRenderer::StartTracking(void)
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking(void)
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render(void)
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	context->RSSetViewports(1,m_viewportOne);
	RenderTheMagic(context);

	context->RSSetViewports(1, m_viewportTwo);
	RenderTheMagic(context);
	

}

void Sample3DSceneRenderer::RenderTheMagic(ID3D11DeviceContext3 * context) {
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));


	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

#pragma region DrawtheCube

	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	//m_constantBufferData.model._14 = 0.0f;
	//m_constantBufferData.model._24 = 6.0f;
	//m_constantBufferData.model._34 = -8.0f;
	// Prepare the constant buffer to send it to the graphics device.
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
#pragma endregion

#pragma region DrawtheSky

	context->UpdateSubresource1(m_SkyconstantBuffer.Get(), 0, NULL, &m_SkyconstantBufferData, 0, 0, 0);
	// Prepare the constant buffer to send it to the graphics device.
	// Each vertex is one instance of the VertexPositionColor struct.
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_SkyvertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_SkyindexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_SkyinputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(m_SkyvertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_SkyconstantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_SkypixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_SkyResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_SkySamplerState.GetAddressOf());
	// Draw the objects.
	context->DrawIndexed(m_SkyindexCount, 0, 0);
#pragma endregion



#pragma region DrawPercy

	context->UpdateSubresource1(m_PercyconstantBuffer.Get(), 0, NULL, &m_PercyconstantBufferData, 0, 0, 0);
	/*m_PercyconstantBufferData.model._24 = 6.5f;
	m_PercyconstantBufferData.model._34 = 2.0f;
*/
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_PercyvertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_PercyindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_PercyinputLayout.Get());
	context->VSSetShader(m_PercyvertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, m_PercyconstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(m_PercypixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_PercyResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_PercySamplerState.GetAddressOf());
	context->DrawIndexed(m_PercyindexCount, 0, 0);

#pragma endregion
#pragma region DrawKriby


	context->UpdateSubresource1(m_ShrekconstantBuffer.Get(), 0, NULL, &m_ShrekconstantBufferData, 0, 0, 0);
	/*m_PercyconstantBufferData.model._34 = 1.0f;*/
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_ShrekvertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_ShrekindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_ShrekinputLayout.Get());
	context->VSSetShader(m_ShrekvertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, m_ShrekconstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(m_ShrekpixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_ShrekResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_ShrekSamplerState.GetAddressOf());
	context->DrawIndexed(m_ShrekindexCount, 0, 0);
#pragma endregion

#pragma region DrawJynx


	context->UpdateSubresource1(m_JynxconstantBuffer.Get(), 0, NULL, &m_JynxconstantBufferData, 0, 0, 0);
	//m_JynxconstantBufferData.model._34 = -4.0f;
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_JynxvertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_JynxindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_JynxinputLayout.Get());
	context->VSSetShader(m_JynxvertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, m_JynxconstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(m_JynxpixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_JynxResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_JynxSamplerState.GetAddressOf());
	context->DrawIndexed(m_JynxindexCount, 0, 0);
#pragma endregion
	context->OMSetRenderTargets(1, m_RTCRenderTargetView.GetAddressOf(), m_deviceResources->GetDepthStencilView());
	context->ClearRenderTargetView(m_RTCRenderTargetView.Get(), DirectX::Colors::SeaGreen);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

#pragma region Rendertotexture
#pragma region DrawtheCube

	/*m_constantBufferData.model._14 = 0.0f;
	m_constantBufferData.model._24 = 6.0f;
	m_constantBufferData.model._34 = -8.0f;*/
	// Prepare the constant buffer to send it to the graphics device.

	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	stride = sizeof(VertexPositionColor);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
#pragma endregion

#pragma region DrawtheSky

	context->UpdateSubresource1(m_SkyconstantBuffer.Get(), 0, NULL, &m_SkyconstantBufferData, 0, 0, 0);
	// Prepare the constant buffer to send it to the graphics device.
	// Each vertex is one instance of the VertexPositionColor struct.
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_SkyvertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_SkyindexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_SkyinputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(m_SkyvertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_SkyconstantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_SkypixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_SkyResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_SkySamplerState.GetAddressOf());
	// Draw the objects.
	context->DrawIndexed(m_SkyindexCount, 0, 0);
#pragma endregion


#pragma region DrawPercy
	//m_PercyconstantBufferData.model._24 = 6.5f;
	//m_PercyconstantBufferData.model._34 = 2.0f;
	context->UpdateSubresource1(m_PercyconstantBuffer.Get(), 0, NULL, &m_PercyconstantBufferData, 0, 0, 0);
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_PercyvertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_PercyindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_PercyinputLayout.Get());
	context->VSSetShader(m_PercyvertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, m_PercyconstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(m_PercypixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_PercyResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_PercySamplerState.GetAddressOf());
	context->DrawIndexed(m_PercyindexCount, 0, 0);

#pragma endregion

#pragma region DrawKriby


	context->UpdateSubresource1(m_ShrekconstantBuffer.Get(), 0, NULL, &m_ShrekconstantBufferData, 0, 0, 0);
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_ShrekvertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_ShrekindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_ShrekinputLayout.Get());
	context->VSSetShader(m_ShrekvertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, m_ShrekconstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(m_ShrekpixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_ShrekResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_ShrekSamplerState.GetAddressOf());
	context->DrawIndexed(m_ShrekindexCount, 0, 0);
#pragma endregion


#pragma region DrawJynx


	//m_JynxconstantBufferData.model._34 = -4.0f;
	context->UpdateSubresource1(m_JynxconstantBuffer.Get(), 0, NULL, &m_JynxconstantBufferData, 0, 0, 0);
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_JynxvertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_JynxindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_JynxinputLayout.Get());
	context->VSSetShader(m_JynxvertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers1(0, 1, m_JynxconstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(m_JynxpixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_JynxResouceView.GetAddressOf());
	context->PSSetSamplers(0, 1, m_JynxSamplerState.GetAddressOf());
	context->DrawIndexed(m_JynxindexCount, 0, 0);
#pragma endregion
#pragma endregion 
#pragma region RTCCube

	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
	context->UpdateSubresource1(m_RTCconstantBuffer.Get(), 0, NULL, &m_RTCconstantBufferData, 0, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	stride = sizeof(VertexPositionUVNormal);
	offset = 0;
	context->IASetVertexBuffers(0, 1, m_RTCvertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_RTCindexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_RTCinputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(m_RTCvertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_RTCconstantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_RTCpixelShader.Get(), nullptr, 0);
	// Draw the objects.

	context->PSSetShaderResources(0, 1, m_RTCSRV.GetAddressOf());
	context->PSSetSamplers(0, 1, m_RTCTexSampleState.GetAddressOf());
	context->DrawIndexed(m_RTCindexCount, 0, 0);
#pragma endregion
}
void Sample3DSceneRenderer::CreateDeviceDependentResources(void)
{
#pragma region Cube
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_vertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_inputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_pixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer));
	});

	/////////////
	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]()
	{
		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] =
		{
			/*0*/{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			/*1*/{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			/*2*/{ XMFLOAT3(0.5f,  -0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			/*3*/{ XMFLOAT3(0.5f,  -0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
			/*4*/{ XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{
			1,0,2,
			2,3,1,

			0,4,2,
			2,4,3,
			3,4,1,
			1,4,0,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]()
	{
		m_loadingComplete = true;
	});
#pragma endregion
#pragma region RTCcube
	// Load shaders asynchronously.
	auto RTCloadVSTask = DX::ReadDataAsync(L"VertexShader.cso");
	auto RTCloadPSTask = DX::ReadDataAsync(L"postProcessPixel.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createRTCVSTask = RTCloadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_RTCvertexShader));

		static const D3D11_INPUT_ELEMENT_DESC RTCvertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(RTCvertexDesc, ARRAYSIZE(RTCvertexDesc), &fileData[0], fileData.size(), &m_RTCinputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createRTCPSTask = RTCloadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_RTCpixelShader));

		CD3D11_BUFFER_DESC RTCconstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&RTCconstantBufferDesc, nullptr, &m_RTCconstantBuffer));
	});
	/////////////


	// Once both shaders are loaded, create the mesh.
	auto RTCcreateCubeTask = (createRTCPSTask && createRTCVSTask).then([this]()
	{
		
		D3D11_TEXTURE2D_DESC desc2d;
		ZeroMemory(&desc2d, sizeof(desc2d));
		Size outputSize = m_deviceResources->GetOutputSize();

		desc2d.MipLevels = 1;
		desc2d.Width = outputSize.Width;
		desc2d.Height = outputSize.Height;
		desc2d.ArraySize = 1;
		desc2d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc2d.SampleDesc.Count = 1;
		desc2d.Usage = D3D11_USAGE_DEFAULT;
		desc2d.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc2d.CPUAccessFlags = 0;
		desc2d.MiscFlags = 0;

		m_deviceResources->GetD3DDevice()->CreateTexture2D(&desc2d, NULL, &m_RTCRenderTargetTextureMap);

		D3D11_RENDER_TARGET_VIEW_DESC targetviewdesc;
		targetviewdesc.Format = desc2d.Format;
		targetviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		targetviewdesc.Texture2D.MipSlice = 0;

		m_deviceResources->GetD3DDevice()->CreateRenderTargetView(m_RTCRenderTargetTextureMap.Get(), &targetviewdesc, &m_RTCRenderTargetView);

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceviewdesc;
		resourceviewdesc.Format = desc2d.Format;
		resourceviewdesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		resourceviewdesc.Texture2D.MostDetailedMip = 0;
		resourceviewdesc.Texture2D.MipLevels = 1;

		m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_RTCRenderTargetTextureMap.Get(), &resourceviewdesc, &m_RTCSRV);
		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionUVNormal RTCcubeVertices[] =
		{
			//Front Face
			/*0*/{ XMFLOAT3(-5.0f, 1.0f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			/*1*/{ XMFLOAT3(-4.0f, 1.0f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			/*2*/{ XMFLOAT3(-5.0f, 0.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			/*3*/{ XMFLOAT3(-4.0f, 0.0f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

			//Right Face
			/*4*/{ XMFLOAT3(-4.0f, 1.0f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			/*5*/{ XMFLOAT3(-4.0f, 1.0f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },
			/*6*/{ XMFLOAT3(-4.0f, 0.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			/*7*/{ XMFLOAT3(-4.0f, 0.0f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f),  XMFLOAT3(1.0f, 0.0f, 0.0f) },

			//Back Face
			/*8*/{ XMFLOAT3(-4.0f, 1.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f) },
			/*9*/{ XMFLOAT3(-5.0f, 1.0f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f) },
			/*10*/{ XMFLOAT3(-4.0f, 0.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			/*11*/{ XMFLOAT3(-5.0f, 0.0f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			//Left Face
			/*12*/{ XMFLOAT3(-5.0f, 1.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			/*13*/{ XMFLOAT3(-5.0f, 1.0f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			/*14*/{ XMFLOAT3(-5.0f, 0.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			/*15*/{ XMFLOAT3(-5.0f, 0.0f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

			//Top Face
			/*16*/{ XMFLOAT3(-5.0f, 1.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(0.0f, 1.0f, 0.0f) },
			/*17*/{ XMFLOAT3(-4.0f, 1.0f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT3(0.0f, 1.0f, 0.0f) },
			/*18*/{ XMFLOAT3(-5.0f, 1.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			/*19*/{ XMFLOAT3(-4.0f, 1.0f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

			//Bottom Face
			/*20*/{ XMFLOAT3(-5.0f, 0.0f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			/*21*/{ XMFLOAT3(-4.0f, 0.0f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			/*22*/{ XMFLOAT3(-5.0f, 0.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT3(0.0f, -1.0f, 0.0f) },
			/*23*/{ XMFLOAT3(-4.0f, 0.0f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f),  XMFLOAT3(0.0f, -1.0f, 0.0f) },
		};
		D3D11_SAMPLER_DESC SamDesc;
		ZeroMemory(&SamDesc, sizeof(SamDesc));
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		

		D3D11_SUBRESOURCE_DATA RTCvertexBufferData = { 0 };
		RTCvertexBufferData.pSysMem = RTCcubeVertices;
		RTCvertexBufferData.SysMemPitch = 0;
		RTCvertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC RTCvertexBufferDesc(sizeof(RTCcubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&RTCvertexBufferDesc, &RTCvertexBufferData, &m_RTCvertexBuffer));

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.0
		static const unsigned short RTCcubeIndices[] =
		{
			0,1,3,
			3,2,0,

			4,5,7,
			7,6,4,

			8,9,11,
			11,10,8,

			12,13,15,
			15,14,12,

			16,17,19,
			19,18,16,

			20,21,23,
			23,22,20,
		};

		m_RTCindexCount = ARRAYSIZE(RTCcubeIndices);

		D3D11_SUBRESOURCE_DATA RTCindexBufferData = { 0 };
		RTCindexBufferData.pSysMem = RTCcubeIndices;
		RTCindexBufferData.SysMemPitch = 0;
		RTCindexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC RTCindexBufferDesc(sizeof(RTCcubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&RTCindexBufferDesc, &RTCindexBufferData, &m_RTCindexBuffer));
	});

	// Once the cube is loaded, the object is ready to be rendered.
	/*createCubeTask.then([this]()
	{
		m_loadingComplete = true;
	});*/
#pragma endregion;

#pragma region Sky
	// Load shaders asynchronously.
	auto SkyloadVSTask = DX::ReadDataAsync(L"SkyVertexShader.cso");
	auto SkyloadPSTask = DX::ReadDataAsync(L"SkyPixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createSkyVSTask = SkyloadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_SkyvertexShader));

		static const D3D11_INPUT_ELEMENT_DESC SkyvertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(SkyvertexDesc, ARRAYSIZE(SkyvertexDesc), &fileData[0], fileData.size(), &m_SkyinputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createSkyPSTask = SkyloadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_SkypixelShader));

		CD3D11_BUFFER_DESC SkyconstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&SkyconstantBufferDesc, nullptr, &m_SkyconstantBuffer));
	});
	/////////////


	// Once both shaders are loaded, create the mesh.
	auto SkycreateCubeTask = (createSkyPSTask && createSkyVSTask).then([this]()
	{
		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionUVNormal SkycubeVertices[] =
		{
			{ XMFLOAT3(-50.5f, -50.5f, -50.5f), XMFLOAT3(-1.0f, -1.0f, -1.0f)  , XMFLOAT3(50.5f, 50.5f, 50.5f)},
			{ XMFLOAT3(-50.5f, -50.5f,  50.5f), XMFLOAT3(-1.0f, -1.0f, 1.0f)   , XMFLOAT3(50.5f, 50.5f,  -50.5f)},
			{ XMFLOAT3(-50.5f,  50.5f, -50.5f), XMFLOAT3(-1.0f, 1.0f, -1.0f)   , XMFLOAT3(50.5f,  -50.5f, 50.5f)},
			{ XMFLOAT3(-50.5f,  50.5f,  50.5f), XMFLOAT3(-1.0f, 1.0f, 1.0f)    , XMFLOAT3(50.5f,  -50.5f,  -50.5f)},
			{ XMFLOAT3(50.5f, -50.5f, -50.5f),  XMFLOAT3(1.0f, -1.0f, -1.0f)   , XMFLOAT3(-50.5f, 50.5f, 50.5f)},
			{ XMFLOAT3(50.5f, -50.5f,  50.5f),  XMFLOAT3(1.0f, -1.0f, 1.0f)    , XMFLOAT3(-50.5f, 50.5f,  -50.5f)},
			{ XMFLOAT3(50.5f,  50.5f, -50.5f),  XMFLOAT3(1.0f, 1.0f, -1.0f)    , XMFLOAT3(-50.5f,  -50.5f, 50.5f)},
			{ XMFLOAT3(50.5f,  50.5f,  50.5f),  XMFLOAT3(1.0f, 1.0f, 1.0f)     , XMFLOAT3(-50.5f,  -50.5f,  -50.5f)}
		};

		D3D11_SUBRESOURCE_DATA SkyvertexBufferData = { 0 };
		SkyvertexBufferData.pSysMem = SkycubeVertices;
		SkyvertexBufferData.SysMemPitch = 0;
		SkyvertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(SkycubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &SkyvertexBufferData, &m_SkyvertexBuffer));

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short SkycubeIndices[] =
		{
			2,1,0, // -x
			2,3,1,

			5,6,4, // +x
			7,6,5,

			1,5,0, // -y
			5,4,0,

			6,7,2, // +y
			7,3,2,

			4,6,0, // -z
			6,2,0,

			3,7,1, // +z
			7,5,1,
		};

		m_SkyindexCount = ARRAYSIZE(SkycubeIndices);

		D3D11_SAMPLER_DESC SkySamDesc;
		ZeroMemory(&SkySamDesc, sizeof(SkySamDesc));
		SkySamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SkySamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SkySamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SkySamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&SkySamDesc, &m_SkySamplerState));
		DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/spacemap.dds", NULL, &m_SkyResouceView));

		D3D11_SUBRESOURCE_DATA SkyindexBufferData = { 0 };
		SkyindexBufferData.pSysMem = SkycubeIndices;
		SkyindexBufferData.SysMemPitch = 0;
		SkyindexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC SkyindexBufferDesc(sizeof(SkycubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&SkyindexBufferDesc, &SkyindexBufferData, &m_SkyindexBuffer));
		
		
	});

#pragma endregion;
#pragma region kirbyShit

	//Shrek Shit
	loadVSTask = DX::ReadDataAsync(L"VertexShader.cso");
	loadPSTask = DX::ReadDataAsync(L"PixelShader.cso");

	auto createShrekVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_ShrekvertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_ShrekinputLayout));
	});

	auto createShrekPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_ShrekpixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_ShrekconstantBuffer));
	});

	auto createShrekTask = (createShrekPSTask && createShrekVSTask).then([this]()
	{
		std::vector<VertexPositionUVNormal> ShrekVertices;
		std::vector<unsigned int> ShrekIndices;
		char * ItsAllOgreNow = "Assets/course_model_new.obj";
		LoadObject(ItsAllOgreNow, ShrekVertices, ShrekIndices);

		D3D11_SAMPLER_DESC SamDesc;
		ZeroMemory(&SamDesc, sizeof(SamDesc));
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&SamDesc, &m_ShrekSamplerState));
		DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/zara.dds", NULL, &m_ShrekResouceView));
	
		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = ShrekVertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * ShrekVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_ShrekvertexBuffer));


		m_ShrekindexCount = ShrekIndices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = ShrekIndices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * ShrekIndices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_ShrekindexBuffer));
	});

#pragma endregion

#pragma region PercyShit
	//Percy Shit
	loadVSTask = DX::ReadDataAsync(L"VertexShader.cso");
	loadPSTask = DX::ReadDataAsync(L"PixelShader.cso");

	auto createPercyVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_PercyvertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_PercyinputLayout));
	});

	auto createPercyPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_PercypixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_PercyconstantBuffer));
	});

	auto createPercyTask = (createPercyPSTask && createPercyVSTask).then([this]()
	{
		std::vector<VertexPositionUVNormal> PercyVertices;
		std::vector<unsigned int> PercyIndices;
		char * ItsAllOgreNow = "Assets/lanky.obj";//AsfStafy00.obj";
		LoadObject(ItsAllOgreNow, PercyVertices, PercyIndices);

		D3D11_SAMPLER_DESC SamDesc;
		ZeroMemory(&SamDesc, sizeof(SamDesc));
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&SamDesc, &m_PercySamplerState));
		DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/lanky.dds", NULL, &m_PercyResouceView));

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = PercyVertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * PercyVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_PercyvertexBuffer));


		m_PercyindexCount = PercyIndices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = PercyIndices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * PercyIndices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_PercyindexBuffer));
	});
#pragma endregion


#pragma region JynxShit

	//Shrek Shit
	loadVSTask = DX::ReadDataAsync(L"VertexShader.cso");
	loadPSTask = DX::ReadDataAsync(L"PixelShader.cso");

	auto createJynxVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_JynxvertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_JynxinputLayout));
	});

	auto createJynxPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_JynxpixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_JynxconstantBuffer));
	});

	auto createJynxTask = (createJynxPSTask && createJynxVSTask).then([this]()
	{
		std::vector<VertexPositionUVNormal> JynxVertices;
		std::vector<unsigned int> JynxIndices;
		char * ItsAllOgreNow = "Assets/jynx.obj";
		LoadObject(ItsAllOgreNow, JynxVertices, JynxIndices);

		D3D11_SAMPLER_DESC SamDesc;
		ZeroMemory(&SamDesc, sizeof(SamDesc));
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&SamDesc, &m_JynxSamplerState));
		DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/jynx.dds", NULL, &m_JynxResouceView));

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = JynxVertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionUVNormal) * JynxVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_JynxvertexBuffer));


		m_JynxindexCount = JynxIndices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = JynxIndices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * JynxIndices.size(), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_JynxindexBuffer));
	});

#pragma endregion

}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources(void)
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	
	m_ShrekvertexShader.Reset();
	m_ShrekinputLayout.Reset();
	m_ShrekpixelShader.Reset();
	m_ShrekconstantBuffer.Reset();
	m_ShrekvertexBuffer.Reset();
	m_ShrekindexBuffer.Reset();

	m_PercyvertexShader.Reset();
	m_PercyinputLayout.Reset();
	m_PercypixelShader.Reset();
	m_PercyconstantBuffer.Reset();
	m_PercyvertexBuffer.Reset();
	m_PercyindexBuffer.Reset();
	
	m_JynxvertexShader.Reset();
	m_JynxinputLayout.Reset();
	m_JynxpixelShader.Reset();
	m_JynxconstantBuffer.Reset();
	m_JynxvertexBuffer.Reset();
	m_JynxindexBuffer.Reset();

	delete m_viewportOne;
	delete m_viewportTwo;
}

#pragma region Loaders 

#pragma warning(disable:4996)  
void DX11UWA::Sample3DSceneRenderer::LoadObject(char * lePath, std::vector<VertexPositionUVNormal>& leVertexs, std::vector<unsigned int>& leIndices)
{
	std::vector<XMFLOAT3> leVertices;
	std::vector<XMFLOAT3> leUVs;
	std::vector<XMFLOAT3> leNormals;
	std::vector<unsigned int> leVIndices;
	std::vector<unsigned int> leUVIndices;
	std::vector<unsigned int> leNIndices;
	float levertexIndex, leuvIndex, lenormalIndex, levertexIndex1, leuvIndex1, lenormalIndex1, levertexIndex2, leuvIndex2, lenormalIndex2 = 0;
	char leBuffer[148];

	std::FILE * leFile = std::fopen(lePath, "r");

	while (!std::feof(leFile))
	{
		std::fscanf(leFile, "%s", leBuffer);

		if (std::strcmp(leBuffer, "v") == 0)
		{
			XMFLOAT3 leVertex;
			fscanf(leFile, "%f %f %f\n", &leVertex.x, &leVertex.y, &leVertex.z);
			leVertices.push_back(leVertex);
		}
		else if (std::strcmp(leBuffer, "vt") == 0)
		{
			XMFLOAT3 leUV;
			fscanf(leFile, "%f %f\n", &leUV.x, &leUV.y);
			leUV.y = 1 - leUV.y;
			leUV.z = 1;
			leUVs.push_back(leUV);
		}
		else if (std::strcmp(leBuffer, "vn") == 0)
		{
			XMFLOAT3 leNormal;
			fscanf(leFile, "%f %f %f\n", &leNormal.x, &leNormal.y, &leNormal.z);
			leNormals.push_back(leNormal);
		}
		else if (std::strcmp(leBuffer, "f") == 0)
		{
			fscanf(leFile, "%f/%f/%f %f/%f/%f %f/%f/%f\n", &levertexIndex, &leuvIndex, &lenormalIndex, &levertexIndex1, &leuvIndex1, &lenormalIndex1, &levertexIndex2, &leuvIndex2, &lenormalIndex2);

			leVIndices.push_back(levertexIndex);
			leVIndices.push_back(levertexIndex1);
			leVIndices.push_back(levertexIndex2);
			leUVIndices.push_back(leuvIndex);
			leUVIndices.push_back(leuvIndex1);
			leUVIndices.push_back(leuvIndex2);
			leNIndices.push_back(lenormalIndex);
			leNIndices.push_back(lenormalIndex1);
			leNIndices.push_back(lenormalIndex2);
		}
	}

	for (int i = 0; i < leVIndices.size(); i++)
	{
		VertexPositionUVNormal temp;
		temp.pos = leVertices[leVIndices[i] - 1];
		temp.uv = leUVs[leUVIndices[i] - 1];
		temp.normal = leNormals[leNIndices[i] - 1];

		leVertexs.push_back(temp);
		leIndices.push_back(i);
	}
}

void DX11UWA::Sample3DSceneRenderer::LoadObjectNoNormal(char * lePath, std::vector<VertexPositionUVNormal>& leVertexs, std::vector<unsigned int>& leIndices)
{
	std::vector<XMFLOAT3> leVertices;
	std::vector<XMFLOAT3> leUVs;
	std::vector<XMFLOAT3> leNormals;
	std::vector<unsigned int> leVIndices;
	std::vector<unsigned int> leUVIndices;
	std::vector<unsigned int> leNIndices;
	float levertexIndex, leuvIndex, lenormalIndex, levertexIndex1, leuvIndex1, lenormalIndex1, levertexIndex2, leuvIndex2, lenormalIndex2 = 0;
	char leBuffer[148];

	std::FILE * leFile = std::fopen(lePath, "r");

	while (!std::feof(leFile))
	{
		std::fscanf(leFile, "%s", leBuffer);

		if (std::strcmp(leBuffer, "v") == 0)
		{
			XMFLOAT3 leVertex;
			fscanf(leFile, "%f %f %f\n", &leVertex.x, &leVertex.y, &leVertex.z);
			leVertices.push_back(leVertex);
		}
		else if (std::strcmp(leBuffer, "vt") == 0)
		{
			XMFLOAT3 leUV;
			fscanf(leFile, "%f %f\n", &leUV.x, &leUV.y);
			leUV.y = 1 - leUV.y;
			leUV.z = 1;
			leUVs.push_back(leUV);
		}
		else if (std::strcmp(leBuffer, "vn") == 0)
		{
			XMFLOAT3 leNormal;
			fscanf(leFile, "%f %f %f\n", &leNormal.x, &leNormal.y, &leNormal.z);
			leNormals.push_back(leNormal);
		}
		else if (std::strcmp(leBuffer, "f") == 0)
		{
			fscanf(leFile, "%f/%f %f/%f %f/%f\n", &levertexIndex, &leuvIndex, &levertexIndex1, &leuvIndex1, &levertexIndex2, &leuvIndex2);

			leVIndices.push_back(levertexIndex);
			leVIndices.push_back(levertexIndex1);
			leVIndices.push_back(levertexIndex2);
			leUVIndices.push_back(leuvIndex);
			leUVIndices.push_back(leuvIndex1);
			leUVIndices.push_back(leuvIndex2);
		}
	}

	for (int i = 0; i < leVIndices.size(); i++)
	{
		VertexPositionUVNormal temp;
		temp.pos = leVertices[leVIndices[i] - 1];
		temp.uv = leUVs[leUVIndices[i] - 1];
		temp.normal = XMFLOAT3{ 1.0f, 1.0f, 1.0f };

		leVertexs.push_back(temp);
		leIndices.push_back(i);
	}
}

#pragma endregion