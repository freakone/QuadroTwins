using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.Windows.Forms.DataVisualization.Charting;

namespace QuadroController
{
    public partial class Form1 : Form
    {
        System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
        public Form1()
        {
            InitializeComponent();
            RefreshCom();
            button3_Click(null, null);
            threadADC = new Thread(new ThreadStart(ADCScan));
            threadADC.IsBackground = true;
            threadADC.Start();

            threadParse = new Thread(new ThreadStart(ParseBuffer));
            threadParse.IsBackground = true;
            threadParse.Start();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            threadParse.Abort();
            threadADC.Abort();
        }

        object lockable = new object();
        SerialPort s;
        SerialPort sp
        {          
            get
            {
                lock (lockable)
                {
                    return s;
                }
            }

            set
            {
                lock (lockable)
                {
                    s = value;
                }

            }
        }

        public static string dec2hascii(int liczba, int length)
        {
            return String.Format("{0:X" + length.ToString() + "}", liczba);
        }

        public static int hascii2dec(string str)
        {
            return Convert.ToInt32(str, 16);
        }


        private void button2_Click(object sender, EventArgs e)
        {
            RefreshCom();
        }

        private void RefreshCom()
        {
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(SerialPort.GetPortNames());
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(sp != null)
            {
                try { sp.Close(); }
                catch { }

                sp = null;
                button1.Text = "Połącz";
            }    
            else
            {
                sp = new SerialPort();
                sp.PortName = comboBox1.Text;
                sp.Parity = Parity.None;
                sp.BaudRate = 115200;
                sp.StopBits = StopBits.One;
                sp.Parity = Parity.None;
                sp.DataBits = 8;
                sp.NewLine = "\n";
                sp.DataReceived += sp_DataReceived;

                try
                {
                    sp.Open();
                    button1.Text = "Rozłącz";
                    groupBox1.Text = sp.PortName;
                }
                catch
                {
                    sp = null;
                }
            }
            

        }

        string buffer = "";
        void sp_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            buffer += sp.ReadExisting();
        }

        private void button8_Click(object sender, EventArgs e)
        {
            sp.Write(new byte[] { 0x01 }, 0, 1);
          /*  Thread.Sleep(1000);
            sp.Write(new byte[] { 0x03 }, 0, 1);*/
        }

        private void button9_Click(object sender, EventArgs e)
        {
            sp.Write(new byte[] { 0x02 }, 0, 1);
        }

        public void AddToSeries(int ser, int x, int y)
        {
            if (this.InvokeRequired)
                this.Invoke(new MethodInvoker(delegate()
                {
                    chart1.Series[ser].Points.AddXY(x, y);

                }));
            else
            {
                chart1.Series[ser].Points.AddXY(x, y);
            }
        }

        delegate bool GetCheckBoxSelectedDelegate(CheckBox c);
        public bool GetCheckBoxSelected(CheckBox c)
        {
            if (this.InvokeRequired)
                return (bool)this.Invoke(new GetCheckBoxSelectedDelegate(GetCheckBoxSelected), new object[] { c });
            else
            {
                return c.Checked;
            }
        }

        int iInterval = 100;
        float chartTime = 0;
        
        delegate void AddChartDelegate();
        public void AddChart()
        {
            if (this.InvokeRequired)
                this.Invoke(new AddChartDelegate(AddChart), new object[] { });
            else
            {
                foreach(float f in fAngles)
               {
                    chart1.Series[0].Points.AddXY(chartTime, f);
                    label10.Text = f.ToString();
                    chartTime += (float)iInterval / 1000;
                }
            }
        }

        delegate void FillFormDelegate();
        public void FillForm()
        {
            if (this.InvokeRequired)
                this.Invoke(new FillFormDelegate(FillForm), new object[] {  });
            else
            {
                numericUpDown1.Value = state.px;
                numericUpDown2.Value = state.ix;
                numericUpDown3.Value = state.dx;

                numericUpDown6.Value = state.py;
                numericUpDown5.Value = state.iy;
                numericUpDown4.Value = state.dy;

                numericUpDown9.Value = state.ax;
                numericUpDown10.Value = state.ay;
                numericUpDown7.Value = state.az;

                label10.Text = state.lipo.ToString();
                label11.Text = state.lipo.ToString();

                numericUpDown8.Value = state.power;

            }
        }

        Thread threadADC;

        private void ADCScan()
        {
            while (true)
            {

                if (GetCheckBoxSelected(checkBox1))
                {
                    sp.Write(new byte[] {0x03 }, 0, 1);                
                   

                }
                Thread.Sleep(iInterval);
            }

        }

        Quadro state;
        Thread threadParse;
        private void ParseBuffer()
        {
            while (true)
            {
                while (!buffer.StartsWith(((char)0xFF).ToString()) && buffer.Length > 0)
                {
                    buffer = buffer.Substring(1);
                }

                while (!buffer.Contains("\n")) ;

                bool update = true;
                switch (buffer[1])
                {
                    case (char)0x20:
                        state.px = (decimal)hascii2dec(buffer.Substring(2, 5)) / 100;
                        state.ix = (decimal)hascii2dec(buffer.Substring(7, 5)) / 100;
                        state.dx = (decimal)hascii2dec(buffer.Substring(12, 5)) / 100;
                        break;

                    case (char)0x21:
                        state.py = (decimal)hascii2dec(buffer.Substring(2, 5)) / 100;
                        state.iy = (decimal)hascii2dec(buffer.Substring(7, 5)) / 100;
                        state.dy = (decimal)hascii2dec(buffer.Substring(12, 5)) / 100;
                        break;

                    case (char)0x22:
                        state.power = hascii2dec(buffer.Substring(2, 5));
                        break;
                    case (char)0x23:
                        state.ax = (decimal)hascii2dec(buffer.Substring(2, 8)) / 100;
                        state.ay = (decimal)hascii2dec(buffer.Substring(10, 8)) / 100;
                        state.az = (decimal)hascii2dec(buffer.Substring(18, 8)) / 100;
                        break;
                    case (char)0x27:

                        fAngles.Clear();
                       fAngles.Add((float)hascii2dec(buffer.Substring(2, 8)) / 100);
                      //fAngles.Add((float)hascii2dec(buffer.Substring(10, 8)) / 100);
                        AddChart();

                        
                        update = false;
                        break;

                    case (char)0x25:
                        state.px = (decimal)hascii2dec(buffer.Substring(2, 5)) / 100;
                        state.ix = (decimal)hascii2dec(buffer.Substring(7, 5)) / 100;
                        state.dx = (decimal)hascii2dec(buffer.Substring(12, 5)) / 100;
                        state.py = (decimal)hascii2dec(buffer.Substring(17, 5)) / 100;
                        state.iy = (decimal)hascii2dec(buffer.Substring(22, 5)) / 100;
                        state.dy = (decimal)hascii2dec(buffer.Substring(27, 5)) / 100;
                        state.power = hascii2dec(buffer.Substring(32, 5));
                        state.temp = hascii2dec(buffer.Substring(37, 5));
                        state.lipo = hascii2dec(buffer.Substring(42, 5));
                        state.ax = (decimal)hascii2dec(buffer.Substring(47, 8)) / 100;
                        state.ay = (decimal)hascii2dec(buffer.Substring(55, 8)) / 100;
                        state.az = (decimal)hascii2dec(buffer.Substring(63, 8)) / 100;
                        break;

                    case (char)0x26:
                        state.temp = hascii2dec(buffer.Substring(2, 5));
                        state.lipo = hascii2dec(buffer.Substring(7, 5));
                        break;

                    default:
                        update = false;
                        break;
                }

                if (update)
                    FillForm();

                while (!buffer.StartsWith("\n") && buffer.Length > 0)
                {
                    buffer = buffer.Substring(1);
                }

                if (buffer.Length > 0)
                    buffer = buffer.Substring(1);

            }

        }

        List<float> fAngles = new List<float>();

        struct Quadro
        {
            public decimal ax;
            public decimal ay;
            public decimal az;
            public decimal px;
            public decimal py;
            public decimal ix;
            public decimal iy;
            public decimal dx;
            public decimal dy;
            public int power;
            public int temp;
            public int lipo;

        }

        private void button5_Click(object sender, EventArgs e)
        {
            List<byte> l = new List<byte>();
            l.Add(0xFF);
            l.Add(0x20);
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown1.Value * 100), 5)));
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown2.Value * 100), 5)));
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown3.Value * 100), 5)));
            l.Add(0x0A);
            sp.Write(l.ToArray(), 0, l.Count);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            List<byte> l = new List<byte>();
            l.Add(0xFF);
            l.Add(0x21);
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown6.Value * 100), 5)));
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown5.Value * 100), 5)));
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown4.Value * 100), 5)));
            l.Add(0x0A);
            sp.Write(l.ToArray(), 0, l.Count);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            List<byte> l = new List<byte>();
            l.Add(0xFF);
            l.Add(0x23);
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown9.Value * 100), 8)));
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown10.Value * 100), 8)));
            l.AddRange(encoding.GetBytes(dec2hascii((int)(numericUpDown7.Value * 100), 8)));
            l.Add(0x0A);
            sp.Write(l.ToArray(), 0, l.Count);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            chart1.Series.Clear();
            Series s = new Series();
            s.ChartType = SeriesChartType.Spline;
            chart1.Series.Add(s);
           
            chartTime = 0;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            Series s = new Series();
            s.ChartType = SeriesChartType.Spline;
            chart1.Series.Insert(0, s);
            chartTime = 0;

        }

       
        private void button10_Click(object sender, EventArgs e)
        {
            List<byte> l = new List<byte>();
            l.Add(0xFF);
            l.Add(0x22);
            l.AddRange(encoding.GetBytes(dec2hascii((int)numericUpDown8.Value, 5)));
            l.Add(0x0A);
            sp.Write(l.ToArray(), 0, l.Count);
        }

        private void button11_Click(object sender, EventArgs e)
        {
            sp.Write(new byte[] {0xFF, 0x25, 0x0A}, 0, 3);
        }

      

    }
}
