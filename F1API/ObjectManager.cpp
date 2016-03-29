#include "StdAfx.h"
#include "ObjectManager.h"


CObjectManager::CObjectManager()
	: pFirstItem( NULL )
{
}


CObjectManager::~CObjectManager()
{
	ITEM *pItem = pFirstItem;

	while ( pItem != NULL )
	{
		pFirstItem = pItem->pSbling;

		free(pItem->pObj);
		free(pItem);

		pItem = pFirstItem;
	}
}


BOOL	CObjectManager::Add(void *pObj)
{
	if ( pObj == NULL )
		return TRUE;

	ITEM *pNewItem = (ITEM*) malloc (sizeof(ITEM));
	if (pNewItem == NULL)
		return FALSE;

	pNewItem->pObj = pObj;
	pNewItem->pSbling = pFirstItem;
	pFirstItem = pNewItem;

	return TRUE;
}


void	CObjectManager::Remove(void *pObj)
{
	if ( pObj == NULL )
		return;

	ITEM *pCurrItem = pFirstItem;
	ITEM *pPrevItem = NULL;

	while ( pCurrItem != NULL )
	{
		if (pCurrItem->pObj == pObj)
			break;

		pPrevItem = pCurrItem;
		pCurrItem = pCurrItem->pSbling;
	}

	if ( pCurrItem != NULL )
	{
		if ( pPrevItem != NULL )
			pPrevItem->pSbling = pCurrItem->pSbling;
		else
			pFirstItem = pCurrItem->pSbling;

		free (pCurrItem->pObj);
		free (pCurrItem);
	}
}


BOOL	CObjectManager::IsExist(void *pObj)
{
	ITEM *pItem = pFirstItem;

	while ( pItem != NULL )
	{
		if ( pItem->pObj == pObj )
			return TRUE;

		pItem = pItem->pSbling;
	}

	return FALSE;
}