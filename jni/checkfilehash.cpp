#include "main.h"
#include "vendor/hash/md5.h"
#include "CheckFileHash.h"

bool CheckFile(mdFile* mdChkFile);

cryptor::string_encryptor SAMPFiles[11] = {	
                  cryptor::create("%sSAMP/main.scm", 16),		
                  cryptor::create("%sSAMP/script.img", 18),
                  cryptor::create("%sSAMP/vehicles.ide", 20), 
                  cryptor::create("%sSAMP/handling.cfg", 20), 
                  cryptor::create("%sSAMP/peds.ide", 16), 
                  cryptor::create("%sSAMP/water.dat", 17), 
                  cryptor::create("%sSAMP/water1.dat", 18), 
                  cryptor::create("%sSAMP/weapon.dat", 18),
                  cryptor::create("%sSAMP/gta.dat", 15),
                  cryptor::create("%sSAMP/tracks2.dat", 19),
	cryptor::create("%sSAMP/tracks4.dat", 19) 
};

/* 
0 - main.scm
1 - script.img
2 - vehicles.ide
3 - handling.cfg
4 - peds.ide
5 - water.dat
6 - water1.dat
7 - weapon.dat
8 - gta.dat
9 - tracks2.dat
10 - tracks4.dat
*/

bool FileCheckSum()
{
	mdFile *mdCheckSumFile = new mdFile; 

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[0].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 3212378963;
	mdCheckSumFile->iCorrectDigestArray[2] = 3226940476;
	mdCheckSumFile->iCorrectDigestArray[3] = 2512197760;
	if(!CheckFile(mdCheckSumFile)) return false;

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[1].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 2761982703;
	mdCheckSumFile->iCorrectDigestArray[2] = 3892831165;
	mdCheckSumFile->iCorrectDigestArray[3] = 2244854639;
	if(!CheckFile(mdCheckSumFile)) return false;

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[2].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 401823104;
	mdCheckSumFile->iCorrectDigestArray[2] = 3800320153;
	mdCheckSumFile->iCorrectDigestArray[3] = 1955768670;
	if(!CheckFile(mdCheckSumFile)) return false;

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[3].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 653641750;
	mdCheckSumFile->iCorrectDigestArray[2] = 3000302120;
	mdCheckSumFile->iCorrectDigestArray[3] = 729365877;
	if(!CheckFile(mdCheckSumFile)) return false;

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[4].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 3877381984;
	mdCheckSumFile->iCorrectDigestArray[2] = 8810720;
	mdCheckSumFile->iCorrectDigestArray[3] = 773282059;
	if(!CheckFile(mdCheckSumFile)) return false;

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[5].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 886429307;
	mdCheckSumFile->iCorrectDigestArray[2] = 1485088839;
	mdCheckSumFile->iCorrectDigestArray[3] = 4135896383;
	if(!CheckFile(mdCheckSumFile)) return false;

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[6].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 3370289477;
	mdCheckSumFile->iCorrectDigestArray[2] = 198015362;
	mdCheckSumFile->iCorrectDigestArray[3] = 4090162369;
	if(!CheckFile(mdCheckSumFile)) return false;

	sprintf(mdCheckSumFile->szFileLocation, SAMPFiles[7].decrypt(), g_pszStorage);
	mdCheckSumFile->iCorrectDigestArray[0] = 0;
	mdCheckSumFile->iCorrectDigestArray[1] = 246293635;
	mdCheckSumFile->iCorrectDigestArray[2] = 2570074771;
	mdCheckSumFile->iCorrectDigestArray[3] = 972499787;
	if(!CheckFile(mdCheckSumFile)) return false;

	return true;
}

bool CheckFile(mdFile* mdChkFile)
{
	FILE* fLocalFile;
	fLocalFile = fopen(mdChkFile->szFileLocation, "r");
	int x;
	unsigned char szBuffer[1024];

	if(!fLocalFile)
	{
		Log("Error trying to load %s", mdChkFile->szFileLocation);
		return false;
	}

	MD5_CTX mdCTX;
	MD5Init(&mdCTX);
	while((x = fread(szBuffer, 1, sizeof(szBuffer), fLocalFile) > 0)) MD5Update(&mdCTX, szBuffer, x);
	MD5Final(mdChkFile->szRawLocalDigest, &mdCTX);

	if(ferror(fLocalFile))
	{
		Log("CRC32 checksum failed: an error occurred checking \'%s\'.", mdChkFile->szFileLocation);
		fclose(fLocalFile);
		return false;
	}
		
	// use following output to retreive md5 uint32 array's in md5sums.html
	//char path[255];
	//sprintf(path, "%sSAMP/md5.log", g_pszStorage);
	//FILE* md5sums = fopen(path, "a");
	//fprintf(md5sums, "\n[%s]: current: %u %u %u %u\n", mdChkFile->szFileLocation, mdCTX.buf[0], mdCTX.buf[1], mdCTX.buf[2], mdCTX.buf[3]);
	//fprintf(md5sums, "[%s]: needed: %u %u %u %u\n", mdChkFile->szFileLocation, mdChkFile->iCorrectDigestArray[0], mdChkFile->iCorrectDigestArray[1], mdChkFile->iCorrectDigestArray[2], mdChkFile->iCorrectDigestArray[3]);
	//fclose(md5sums);

	if((mdChkFile->iCorrectDigestArray[0] != mdCTX.buf[0]) || (mdChkFile->iCorrectDigestArray[1] != mdCTX.buf[1]) || (mdChkFile->iCorrectDigestArray[2] != mdCTX.buf[2]) || (mdChkFile->iCorrectDigestArray[3] != mdCTX.buf[3]))
	{
		Log("CRC32 checksum failed: \'%s\'.", mdChkFile->szFileLocation);
		fclose(fLocalFile);
		return false;
	}

	fclose(fLocalFile);
	return true;
}