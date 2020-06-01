#include <list>

template<typename T, size_t buffer_max_size_>
class CircularQueue
{
	typedef typename std::list<T>::iterator iterator;
	typedef typename std::list<T>::const_iterator const_iterator;
public:
	CircularQueue()
	{
		m_iterator_head_ = m_data_.end();
		m_iterator_tail_ = m_data_.end();
	}

	~CircularQueue() = default;

	constexpr size_t MaxSize() const noexcept
	{
		return buffer_max_size_;
	}

	void Push(const T& v)
	{
		if (m_data_.size() < buffer_max_size_)
		{
			m_data_.emplace_back(v);

			if (m_data_.size() > 1)
			{
				++m_iterator_tail_;
			}
			else
			{
				m_iterator_head_ = m_iterator_tail_ = m_data_.begin();
			}
		}
		else
		{
			if (std::next(m_iterator_head_) != m_data_.end())
			{
				++m_iterator_head_;
			}
			else
			{
				m_iterator_head_ = m_data_.begin();
			}

			if (std::next(m_iterator_tail_) != m_data_.end())
			{
				++m_iterator_tail_;
			}
			else
			{
				m_iterator_tail_ = m_data_.begin();
			}

			*m_iterator_tail_ = v;
		}
	}

	void Clear()
	{
		m_data_.clear();
		m_iterator_head_ = m_data_.end();
		m_iterator_tail_ = m_data_.end();
	}

	iterator Head() noexcept
	{
		return m_iterator_head_;
	}

	iterator Tail() noexcept
	{
		return m_iterator_tail_;
	}

	iterator Begin() noexcept
	{
		return m_data_.begin();
	}

	iterator End() noexcept
	{
		return m_data_.end();
	}

	const_iterator Head() const noexcept
	{
		return m_iterator_head_;
	}

	const_iterator Tail() const noexcept
	{
		return m_iterator_tail_;
	}

	const_iterator Begin() const noexcept
	{
		return m_data_.begin();
	}

	const_iterator End() const noexcept
	{
		return m_data_.end();
	}

	template<typename F>
	void ForEach(F f)
	{
		auto itrHead = Head();
		auto itrTail = Tail();
		while (itrHead != End())
		{
			f(*itrHead);
			if (itrHead == itrTail) break;

			++itrHead;

			if (itrHead == End())
			{
				itrHead = Begin();
			}
		}
	}

private:
	iterator m_iterator_head_;

	iterator m_iterator_tail_;

	std::list<T> m_data_;
};