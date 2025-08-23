#pragma once
#include "framework.h"
#include "Helper/comdef.h"
#include "Helper/WinRTHelper.h"
namespace MBox
{
	struct VisualTreeWatcher : implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
	{
	public:
		VisualTreeWatcher(com_ptr<IUnknown> site);
		VisualTreeWatcher(const VisualTreeWatcher&) = delete;
		VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;
		VisualTreeWatcher(VisualTreeWatcher&&) = delete;
		VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;
		~VisualTreeWatcher();
		template <typename T = ::IInspectable>
		auto FromHandle(InstanceHandle handle)
		{
			com_ptr<::IInspectable> obj = nullptr;
			m_xamlDiagnostics->GetIInspectableFromHandle(
				handle, obj.put()
			);
			return obj.as<T>();
		}
		template <typename T = ::IInspectable>
		auto FromIInspectable(com_ptr<T> obj)
		{
			InstanceHandle handle = 0;
			check_hresult(
				m_xamlDiagnostics->GetHandleFromIInspectable(obj.get(), &handle)
			);
			return handle;
		}
		auto FromIInspectable(::IInspectable* obj)
		{
			InstanceHandle handle = 0;
			check_hresult(
				m_xamlDiagnostics->GetHandleFromIInspectable(obj, &handle)
			);
			return handle;
		}
		auto GetXamlDiagnostics() const { return m_xamlDiagnostics; }
		using PropertyCallback = std::function<bool(const PropertyChainValue& property)>;
		using PropertySourceCallback = std::function<bool(const PropertyChainSource& source)>;
		IFACEMETHODIMP WalkInstanceHandle(InstanceHandle handle, PropertyCallback propertyCallback, PropertySourceCallback sourceCallback = nullptr);
	private:
		HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
			ParentChildRelation relation,
			VisualElement element,
			VisualMutationType mutationType
		) override;
		HRESULT STDMETHODCALLTYPE OnElementStateChanged(
			InstanceHandle element,
			VisualElementState elementState,
			LPCWSTR context
		) noexcept override;
		com_ptr<IXamlDiagnostics> m_xamlDiagnostics = nullptr;
	};
}