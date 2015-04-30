using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using System.IO;
using MongoDB.Bson;
using MongoDB.Driver;
using MongoDB.Driver.Builders;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Schema;

namespace SimulatorGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

            string arguments = "--2D";

            arguments += " --numOfSimulators=" + NumOfSimulators.Text;
            arguments += " --numOfParticles=" + ParticleCount.Text;
            arguments += " --maxNoise=" + MaxNoise.Text;
            arguments += " --noiseStep=" + NoiseStep.Text;
            arguments += " --minNoise=" + MinNoise.Text;
            arguments += " --sizeX=" + SizeX.Text;
            arguments += " --sizeY=" + SizeY.Text;


            if (File.Exists(@"AllDataFile.txt"))
            {
                FileStream fs = new FileStream(@"AllDataFile.txt", FileMode.Open, FileAccess.ReadWrite);
                fs.SetLength(fs.Length - 1);
                fs.Position = fs.Length;
                StreamWriter wr = new StreamWriter(fs);
                wr.Write(',');
                wr.Close();
            }
            else File.WriteAllText(@"AllDataFile.txt", "[");

            Process p = new Process()
            {
                StartInfo =
                {
                    UseShellExecute = false,
                    FileName = "vNextSimulator.exe",
                    Arguments = arguments
                },
                EnableRaisingEvents = true
            };

            p.Start();

            p.Exited += p_Exited;
        }

        void p_Exited(object sender, EventArgs e)
        {
            if (File.Exists(@"AllDataFile.txt"))
            {
                FileStream fs = new FileStream(@"AllDataFile.txt", FileMode.Open, FileAccess.ReadWrite);
                fs.Position = fs.Length-1;
                StreamReader rd = new StreamReader(fs);
                if (rd.Read() == ',')
                {
                    fs.Position = fs.Position - 1;
                }
                StreamWriter wr = new StreamWriter(fs);
                
                wr.Write(']');
                wr.Close();
            }
            SaveDataToDB();
        }

        void SaveDataToDB()
        {
            JArray data;

            using (StreamReader s = File.OpenText(@"AllDataFile.txt"))
            {
                using (JsonTextReader reader = new JsonTextReader(s))
                {
                    data = ((JArray)JToken.ReadFrom(reader));
                }
            }

            var entity = new SimulationData(data.Children<JObject>().First());

            var connectionString = "mongodb://localhost";
            var client = new MongoClient(connectionString);
            var server = client.GetServer();
            var database = server.GetDatabase("test");
            database.Drop();
            var collection = database.GetCollection<SimulationData>("simulations");

            collection.Insert(entity);

            var Guid = entity.Guid;
            var query = Query<SimulationData>.EQ(e => e.Guid, Guid);
            var entity1 = collection.FindOne(query);
            MessageBox.Show(entity1.Guid.ToString());
        }
    }
}
