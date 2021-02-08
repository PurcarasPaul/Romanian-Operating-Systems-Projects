#undef UNICODE
#include <windows.h>
#include <stdio.h>

ULARGE_INTEGER ulFreeForUser, ulTotal, ulFree;

void DriveInfo(LPTSTR lpszRootPathName)
{
	// Variabile pentru informatii despre unitate
	int nDriveType;
	char p[300];

	// Variabile pentru informatii despre volum
	char szBuf[400];
	char lpVolumeNameBuffer[200];
	DWORD dwVolumeSerialNumber, dwMaximumComponentLength;
	DWORD dwFileSystemFlags;
	char lpFileSystemNameBuffer[50];

	// Variabile pentru informatii despre spatiu liber
	DWORD dwSectorsPerCluster, dwBytesPerSector;
	DWORD dwFreeClusters, dwClusters;
	DWORD dwTotalDiskSpace, dwFreeDiskSpace, dwUsedDiskSpace;

	// Obtine informatii despre tipul unitatii
	nDriveType = GetDriveType(lpszRootPathName);

	/*
			 DRIVE_UNKNOWN         The drive type cannot be determined.
			 DRIVE_NO_ROOT_DIR     The root directory does not exist.
			 DRIVE_REMOVABLE       The disk can be removed from the drive.
			 DRIVE_FIXED           The disk cannot be removed from the drive.
			 DRIVE_REMOTE          The drive is a remote (network) drive.
			 DRIVE_CDROM           The drive is a CD-ROM drive.
			 DRIVE_RAMDISK         The drive is a RAM disk.
	*/

	switch (nDriveType) {
	case 0:
		strcpy_s(p, "Nu se poate determina");
		break;

	case 1:
		strcpy_s(p, "Nu exista aceasta cale");
		break;

	case DRIVE_REMOVABLE:
		strcpy_s(p, "disketa");
		break;

	case DRIVE_FIXED:
		strcpy_s(p, "disc fix");
		break;

	case DRIVE_REMOTE:
		strcpy_s(p, "retea");
		break;

	case DRIVE_CDROM:
		strcpy_s(p, "CD-ROM");
		break;

	case DRIVE_RAMDISK:
		strcpy_s(p, "RAM disc");
		break;

	default:
		strcpy_s(p, "necunoscut");
		break;
	}

	printf_s("Tipul unitatii: %s\n", p);



	// Obtine informatii despre volum
	if (GetVolumeInformation(lpszRootPathName, //numele driver-ului (dir rad)
		lpVolumeNameBuffer, // pointer la bufferul pentru eticheta de volum
		200, // dimensiunea pointerului la bufferul pentru eticheta de volum
		&dwVolumeSerialNumber, // pointer la buffer-ul pentru nr. serial
		&dwMaximumComponentLength, // adresa pentru dimensiune maximã nume 
		//Fisier
		&dwFileSystemFlags, // adresã flaguri sistem de fiºiere
		lpFileSystemNameBuffer, // pointer la buffer-ul pentru nume sistem fiºiere
		50 // dimensiunea lui lpFileSystemNameBuffer
	))

	{

		sprintf_s(szBuf, "Eticheta volum: %s\n Serial number: %x\n Nume fisiere (max): %u\n",
			lpVolumeNameBuffer, dwVolumeSerialNumber,
			dwMaximumComponentLength);

		if (dwFileSystemFlags & FS_CASE_IS_PRESERVED) //numele fisierelor contin litere mari si mici
		{
			strcat_s(szBuf, "FS_CASE_IS_PRESERVED");
			strcat_s(szBuf, "\n");
		}

		if (dwFileSystemFlags & FS_CASE_SENSITIVE) //se face distinctie intre litere mari si mici la cautarea fisierelor
		{
			strcat_s(szBuf, "FS_CASE_SENSITIVE");
			strcat_s(szBuf, "\n");
		}

		if (dwFileSystemFlags & FS_UNICODE_STORED_ON_DISK) //suport UNICODE in numele de fisiere
		{
			strcat_s(szBuf, "FS_UNICODE_STORED_ON_DISK");
			strcat_s(szBuf, "\n");
		}

		strcat_s(szBuf, "Sistem de fisiere: ");
		strcat_s(szBuf, lpFileSystemNameBuffer);
	}
	else
	{
		strcpy_s(szBuf, "NO VOLUME INFO");
	}

	printf_s("Informatii despre volum: \n %s \n", szBuf);

	// Obtine informatii despre spatiu liber.

	if (GetDiskFreeSpace(lpszRootPathName,
		&dwSectorsPerCluster, &dwBytesPerSector,
		&dwFreeClusters, &dwClusters)) {
		sprintf_s(szBuf, "Sectoare/cluster: %u\n Bytes/sector: %u\n Clusteri liberi: %u\n Total clusteri: %u",
			dwSectorsPerCluster, dwBytesPerSector,
			dwFreeClusters, dwClusters);

		strcat_s(szBuf, "\n");
		printf_s("Caracteristici fizice: \n %s ", szBuf);
		dwTotalDiskSpace =
			dwSectorsPerCluster * dwBytesPerSector * dwClusters;

		dwFreeDiskSpace =
			dwSectorsPerCluster * dwBytesPerSector * dwFreeClusters;

		dwUsedDiskSpace =
			dwSectorsPerCluster * dwBytesPerSector *
			(dwClusters - dwFreeClusters);

		printf_s("\n****1****\n");

		sprintf_s(szBuf, "Total spatiu: %u\n Spatiu liber: %u\n Spatiu ocupat: %u\n",
			dwTotalDiskSpace, dwFreeDiskSpace, dwUsedDiskSpace);
	}
	else
	{
		strcpy_s(szBuf, "NO\nDISK\nSPACE\nINFO");
	}

	printf_s("%s \n", szBuf);

	if (GetDiskFreeSpaceEx(lpszRootPathName, &ulFreeForUser, &ulTotal, &ulFree)) {
		sprintf_s(szBuf, "\n********\nTotal spatiu disponibil: %I64u\n"
			"Total spatiu liber pentru user: %I64u\n"
			"Spatiu liber total: %I64u\n",
			ulTotal.QuadPart, ulFreeForUser.QuadPart, ulFree.QuadPart);
	}
	else
	{
		strcpy_s(szBuf, "NO\nDISK\nSPACE\nINFO");
	}

	printf_s("%s \n", szBuf);

}

BOOL DoesDriveExist(char cDriveLetter)
{
	cDriveLetter = (char)CharUpper((LPSTR)cDriveLetter);
	return(GetLogicalDrives() & (1 << (cDriveLetter - 'A')));
}

int main()
{
	char NumeDir[255] = "\0";
	char Disk;

	printf("Dati disk-ul: ");
	scanf("%c", &Disk);

	if (DoesDriveExist(Disk))
	{
		NumeDir[0] = Disk;
		strcat(NumeDir, ":\\");
		DriveInfo(NumeDir);
	}
	else
		printf("Nu exista disk-ul %c", Disk);

	printf("\n");
	system("pause");

	return 0;
}