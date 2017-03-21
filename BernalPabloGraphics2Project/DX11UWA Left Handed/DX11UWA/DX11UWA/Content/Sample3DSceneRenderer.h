
#pragma once
#include <vector>
#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "Common\DDSTextureLoader.h"


namespace DX11UWA
{
	
	// This sample rederer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources(void);
		void CreateWindowSizeDependentResources(void);
		void ReleaseDeviceDependentResources(void);
		void Update(DX::StepTimer const& timer);
		void Render(void);
		void StartTracking(void);
		void TrackingUpdate(float positionX);
		void StopTracking(void);
		inline bool IsTracking(void) { return m_tracking; }
		void LoadObject(char * lePath, std::vector<VertexPositionUVNormal>& leVertexs, std::vector<unsigned int>& leIndices);
		void LoadObjectNoNormal(char * lePath, std::vector<VertexPositionUVNormal>& leVertexs, std::vector<unsigned int>& leIndices);
		void RenderTheMagic(ID3D11DeviceContext3 * context);
		// Helper functions for keyboard and mouse input
		void SetKeyboardButtons(const char* list);
		void SetMousePosition(const Windows::UI::Input::PointerPoint^ pos);
		void SetInputDeviceData(const char* kb, const Windows::UI::Input::PointerPoint^ pos);

		float m_nearP = 0.1;
		float m_farP = 100.0f;
		float m_FOV = 70.0f;
	private:
		void Rotate(float radians);
		void UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;



		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_RTCinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_RTCvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_RTCindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_RTCvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_RTCpixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_RTCconstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>       m_RTCRenderTargetTextureMap;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_RTCRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_RTCSRV;

		Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_RTCTexSampleState;

		//Kirby
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_ShrekinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_ShrekvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_ShrekindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_ShrekvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_ShrekpixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_ShrekconstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_ShrekSamplerState;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShrekResouceView;
		
		//GB
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_GBinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_GBvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_GBindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_GBvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_GBpixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_GBconstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_GBSamplerState;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_GBResouceView;

		//Percy
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_PercyinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PercyvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PercyindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_PercyvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PercypixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_PercyconstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_PercySamplerState;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_PercyResouceView;

		//Jynx
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_JynxinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_JynxvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_JynxindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_JynxvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_JynxpixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_JynxconstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_JynxSamplerState;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_JynxResouceView;

		//Kirby
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_SkyinputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_SkyvertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_SkyindexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_SkyvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_SkypixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_SkyconstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_SkySamplerState;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SkyResouceView;
		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_RTCconstantBufferData;
		uint32	m_RTCindexCount;


		//Shrek
		ModelViewProjectionConstantBuffer	m_ShrekconstantBufferData;
		uint32	m_ShrekindexCount;


		//GB
		ModelViewProjectionConstantBuffer	m_GBconstantBufferData;
		uint32	m_GBindexCount;

		//Percy
		ModelViewProjectionConstantBuffer	m_PercyconstantBufferData;
		uint32	m_PercyindexCount;

		//Percy
		ModelViewProjectionConstantBuffer	m_JynxconstantBufferData;
		uint32	m_JynxindexCount;

		//Sky
		ModelViewProjectionConstantBuffer	m_SkyconstantBufferData;
		uint32	m_SkyindexCount;
		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		// Data members for keyboard and mouse input
		char	m_kbuttons[256];
		Windows::UI::Input::PointerPoint^ m_currMousePos;
		Windows::UI::Input::PointerPoint^ m_prevMousePos;

		// Matrix data member for the camera
		DirectX::XMFLOAT4X4 m_camera;
		DirectX::XMFLOAT4X4 m_cameratt;

		
		// viewports
		D3D11_VIEWPORT * m_viewportOne;
		D3D11_VIEWPORT * m_viewportTwo;
		D3D11_VIEWPORT * m_viewportThree;
		bool ActiveViews = false;

		//// transparency
		//Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterstatefront;
		//Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterstateback;
		//Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterstatereset;
		//Microsoft::WRL::ComPtr<ID3D11BlendState> blenderstate;
		//Microsoft::WRL::ComPtr<ID3D11BlendState> blenderstatereset;
	};
}

