#pragma once
template<typename T>
class Singleton
{
protected:
	Singleton() = default;

	virtual ~Singleton() = default;

public:
	static T& GetInstance()
	{
		if (!m_instance_) m_instance_ = new T{};
		static auto* ptr = &m_helper_;
		return *m_instance_;
	}

private:
	static T* m_instance_;

private:
	class Helper
	{
	public:
		Helper()
		{
			GetInstance();
		}

		~Helper()
		{
			if (Singleton<T>::m_instance_)
			{
				delete Singleton<T>::m_instance_;
				Singleton<T>::m_instance_ = nullptr;
			}
		}
	};

	static Helper m_helper_;
};

template<typename T>
T* Singleton<T>::m_instance_ = nullptr;

template<typename T>
typename Singleton<T>::Helper Singleton<T>::m_helper_;
