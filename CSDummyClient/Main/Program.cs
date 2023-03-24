// C++ IOCP 서버의 IP 주소와 포트 번호
using CSDummyClient;
using CSDummyClient.GameContents;
using CSDummyClient.Main;

System.Threading.Thread.Sleep(1000);

Connector connector = new Connector();
connector.Connect(
    "127.0.0.1",
    7777,
    () => { return new ServerSession(); },
    1);

Managers.Instance.Init();

while (true)
{
    try
    {
        Managers.Instance.Update();
    }
    catch (Exception e)
    {
        Console.WriteLine(e.ToString());
    }

    Thread.Sleep(250);
}