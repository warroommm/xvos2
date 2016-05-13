
#ifndef _MENU_HEADER_
#define _MENU_HEADER_

  #define MRU_REGISTRY_NAME 	"Software\\National Instruments\\XVosMRU\\"
//  #define MRU_REGISTRY_NAME 	"XVosmru.ini"

#define MRU_MAX_ITEMS       	5
#define MRU_MAX_PATH_SIZE   	32
#define MRU_SECTIONNAME     	"FileMenuList"
#define MRU_TAGPREFIX       	"Filename"

void InitializeMRU(int panelHandle);
void SaveMRUList(void);
void AddUIRtoTree(char* tempUIRPath);
void CVICALLBACK DeferredMenuCB (void* callbackData);

int LoadUIR_safe(char* fileName);

#endif //_MENU_HEADER_






