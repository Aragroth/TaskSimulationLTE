import pandas as pd


def calculate_throughput(file_path, sim_start=0.2, sim_stop=10.0):
    simulation_time = sim_stop - sim_start
    
    df = pd.read_csv(file_path, delimiter="\t", index_col=False)
    throughput_data = df.groupby("IMSI").agg({"TxBytes": "sum", "RxBytes": "sum"}).reset_index()

    throughput_data["UL_Throughput_bps"] = (throughput_data["TxBytes"] * 8) / simulation_time
    throughput_data["DL_Throughput_bps"] = (throughput_data["RxBytes"] * 8) / simulation_time

    print(throughput_data[["IMSI", "UL_Throughput_bps", "DL_Throughput_bps"]])

    return throughput_data 


if __name__ == "__main__":
    file_path_dl = "key-stats/DlRlcStats.txt"
    file_path_up = "key-stats/UlRlcStats.txt"
    
    print(f"\nThroughput {file_path_dl} results (bps):")
    calculate_throughput(file_path_dl)

    print(f"\nThroughput {file_path_up} results (bps):")
    calculate_throughput(file_path_up)

