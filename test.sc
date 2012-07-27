/* comment test */
namespace Example.Tests
{
    static class Test1
    {
        static int _i;
    }

    static class Test2
    {
        static int _j = 1+0x2+0b11+4;
        static byte _k;
        static int _u = 1000;

        static int testCall()
        {
            return _u+_u;
        }

        static void main()
        {
            int tt = testCall();
            _j = 10 + tt;
        }
    }
}
