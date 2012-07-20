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

        static void main()
        {
            _j = 100+_u+_u;
            _k = 12'b;
        }
    }
}
