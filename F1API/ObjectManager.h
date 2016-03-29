#pragma once

class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

	BOOL	Add(void *pObj);
	void	Remove(void *pObj);

	BOOL	IsExist(void *pObj);

private:
	struct ITEM
	{
		void *pObj;
		ITEM *pSbling;
	};

	ITEM *pFirstItem;
};

