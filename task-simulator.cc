#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TaskSimulator");

int
main(int argc, char* argv[])
{
    NS_LOG_UNCOND("Task simulation started");

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    // to build full EPC network block with S1 interface
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");

    NodeContainer enbNodes, ueNodes;
    enbNodes.Create(1);
    ueNodes.Create(2);

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);
    mobility.Install(ueNodes);

    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    InternetStackHelper internet;
    internet.Install(ueNodes);

    Ipv4InterfaceContainer ueIpIfaces;
    ueIpIfaces = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

    lteHelper->Attach(ueLteDevs, enbLteDevs.Get(0));

    // Key Performance Indicators
    lteHelper->EnableRlcTraces();
    lteHelper->EnableMacTraces();

    // setup UDP servers
    uint16_t dlPort = 1000;
    for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
    {
        UdpServerHelper udpServer(dlPort);
        ApplicationContainer serverApps = udpServer.Install(ueNodes.Get(i));
        serverApps.Start(Seconds(0.1));
        serverApps.Stop(Seconds(10.0));
    }

    // setup UDP clients full-buffer
    for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
    {
        uint32_t anotherUeNodeId = ueNodes.GetN() - 1 - i;
        UdpClientHelper udpClient(ueIpIfaces.GetAddress(anotherUeNodeId), dlPort);
        udpClient.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
        udpClient.SetAttribute("PacketSize", UintegerValue(1024));
        udpClient.SetAttribute("MaxPackets", UintegerValue(0));
        ApplicationContainer clientApps = udpClient.Install(ueNodes.Get(i));
        clientApps.Start(Seconds(0.2));
        clientApps.Stop(Seconds(10.0));
    }

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
