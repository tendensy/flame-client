class CTxdStore
{
public:
    static void PushCurrentTxd()
    {
        // CTxdStore::PushCurrentTxd
        (( void (*)())(g_libGTASA + 0x55BD6C + 1))();
    }

    static void PopCurrentTxd()
    {
        // CTxdStore::PopCurrentTxd
        (( void (*)())(g_libGTASA + 0x55BDA8 + 1))();
    }

    static int FindTxdSlot(char const* name)
    {
        // CTxdStore::FindTxdSlot
        return (( int (*)(char const*))(g_libGTASA + 0x55BB84 + 1))(name);
    }

    static void SetCurrentTxd(int index)
    {
        // CTxdStore::SetCurrentTxd
        (( void (*)(int, uint32_t))(g_libGTASA + 0x55BCDC + 1))(index, 0);
    }

    static int AddTxdSlot(const char *szTxdName, const char *szTexdb)
    {
        // CTxdStore::AddTxdSlot
        return (( int (*)(const char *, const char *, int))(g_libGTASA + 0x55B968 + 1))(szTxdName, szTexdb, 0);
    }

    static void AddRef(int iTxdSlot)
    {
        // CTxdStore::AddRef
        (( void (*)(int))(g_libGTASA + 0x55BE18 + 1))(iTxdSlot);
    }
};