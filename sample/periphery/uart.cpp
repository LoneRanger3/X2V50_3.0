#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Log.h"
#include "io.h"
#include "uart.h"
#include "XMThread.h"

static int fd_uart[3] = {-1, -1, -1};
const char dev_uart[3][16] = {"/dev/ttyAMA0", "/dev/ttyAMA1", "/dev/ttyAMA2"};

int SetAttribute(int dev_fd, COMM_ATTR *pattr)
{
	struct termios opt;
	COMM_ATTR *attr = pattr;

	if (dev_fd < 0)
	{
		return -1;
	}

	memset(&opt, 0, sizeof(struct termios));
	tcgetattr(dev_fd, &opt);
	cfmakeraw(&opt);			/* set raw mode	*/

	/*
	* set speed
	*/
	switch (attr->baudrate)
	{
		case 50:
			cfsetispeed(&opt, B50);
			cfsetospeed(&opt, B50);
			break;
		case 75:
			cfsetispeed(&opt, B75);
			cfsetospeed(&opt, B75);
			break;
		case 110:
			cfsetispeed(&opt, B110);
			cfsetospeed(&opt, B110);
			break;
		case 134:
			cfsetispeed(&opt, B134);
			cfsetospeed(&opt, B134);
			break;
		case 150:
			cfsetispeed(&opt, B150);
			cfsetospeed(&opt, B150);
			break;
		case 200:
			cfsetispeed(&opt, B200);
			cfsetospeed(&opt, B200);
			break;
		case 300:
			cfsetispeed(&opt, B300);
			cfsetospeed(&opt, B300);
			break;
		case 600:
			cfsetispeed(&opt, B600);
			cfsetospeed(&opt, B600);
			break;
		case 1200:
			cfsetispeed(&opt, B1200);
			cfsetospeed(&opt, B1200);
			break;
		case 1800:
			cfsetispeed(&opt, B1800);
			cfsetospeed(&opt, B1800);
			break;
		case 2400:
			cfsetispeed(&opt, B2400);
			cfsetospeed(&opt, B2400);
			break;
		case 4800:
			cfsetispeed(&opt, B4800);
			cfsetospeed(&opt, B4800);
			break;
		case 9600:
			cfsetispeed(&opt, B9600);
			cfsetospeed(&opt, B9600);
			break;
		case 19200:
			cfsetispeed(&opt, B19200);
			cfsetospeed(&opt, B19200);
			break;
		case 38400:
			cfsetispeed(&opt, B38400);
			cfsetospeed(&opt, B38400);
			break;
		case 57600:
			cfsetispeed(&opt, B57600);
			cfsetospeed(&opt, B57600);
			break;
		case 115200:
			cfsetispeed(&opt, B115200);
			cfsetospeed(&opt, B115200);
			break;
		case 230400:
			cfsetispeed(&opt, B230400);
			cfsetospeed(&opt, B230400);
			break;
		case 460800:
			cfsetispeed(&opt, B460800);
			cfsetospeed(&opt, B460800);
			break;
		case 500000:
			cfsetispeed(&opt, B500000);
			cfsetospeed(&opt, B500000);
			break;
		case 576000:
			cfsetispeed(&opt, B576000);
			cfsetospeed(&opt, B576000);
			break;
		case 921600:
			cfsetispeed(&opt, B921600);
			cfsetospeed(&opt, B921600);
			break;
		case 1000000:
			cfsetispeed(&opt, B1000000);
			cfsetospeed(&opt, B1000000);
			break;
		case 1152000:
			cfsetispeed(&opt, B1152000);
			cfsetospeed(&opt, B1152000);
			break;
		case 1500000:
			cfsetispeed(&opt, B1500000);
			cfsetospeed(&opt, B1500000);
			break;
		case 2000000:
			cfsetispeed(&opt, B2000000);
			cfsetospeed(&opt, B2000000);
			break;
		case 2500000:
			cfsetispeed(&opt, B2500000);
			cfsetospeed(&opt, B2500000);
			break;
		case 3000000:
			cfsetispeed(&opt, B3000000);
			cfsetospeed(&opt, B3000000);
			break;
		case 3500000:
			cfsetispeed(&opt, B3500000);
			cfsetospeed(&opt, B3500000);
			break;
		case 4000000:
			cfsetispeed(&opt, B4000000);
			cfsetospeed(&opt, B4000000);
			break;
		default:
			printf("unsupported baudrate %d\n", attr->baudrate);
			break;
	}

	/*
	* set parity
	*/
	switch (attr->parity)
	{
		case COMM_NOPARITY:		/* none			*/
			opt.c_cflag &= ~PARENB;	/* disable parity	*/
			opt.c_iflag &= ~INPCK;	/* disable parity check	*/
			break;
		case COMM_ODDPARITY:		/* odd			*/
			opt.c_cflag |= PARENB;	/* enable parity	*/
			opt.c_cflag |= PARODD;	/* odd			*/
			opt.c_iflag |= INPCK;	/* enable parity check	*/
			break;
		case COMM_EVENPARITY:		/* even			*/
			opt.c_cflag |= PARENB;	/* enable parity	*/
			opt.c_cflag &= ~PARODD;	/* even			*/
			opt.c_iflag |= INPCK;	/* enable parity check	*/
		default:
			printf("unsupported parity %d\n", attr->parity);
			break;
	}

	/*
	* set data bits
	*/
	opt.c_cflag &= ~CSIZE;
	switch (attr->databits)
	{
		case 5:
			opt.c_cflag |= CS5;
			break;
		case 6:
			opt.c_cflag |= CS6;
			break;
		case 7:
			opt.c_cflag |= CS7;
			break;
		case 8:
			opt.c_cflag |= CS8;
			break;
		default:
			printf("unsupported data bits %d\n", attr->databits);
			break;
	}

	/*
	* set stop bits
	*/
	opt.c_cflag &= ~CSTOPB;
	switch (attr->stopbits)
	{
		case COMM_ONESTOPBIT:
			opt.c_cflag &= ~CSTOPB;
			break;
/*		case COMM_ONE5STOPBIT:
			break;
*/
		case COMM_TWOSTOPBITS:
			opt.c_cflag |= CSTOPB;
			break;
		default:
			printf("unsupported stop bits %d\n", attr->stopbits);
			break;
	}
	opt.c_cc[VTIME]	= 0;
	opt.c_cc[VMIN]	= 1;			/* block until data arrive */

	tcflush(dev_fd, TCIOFLUSH);
	if (tcsetattr(dev_fd, TCSANOW, &opt) < 0)
	{
		printf("tcsetattr\n");
		return -1;
	}

	return 0;
} /* end SetAttribute */


int UartOpen(int index)
{	
	if(index < 0 || index > 2)
	{
		printf("no uart dev %d\n", index);
		return -1;
	}
	if (fd_uart[index] < 0)
	{
		if(index == 2)
		{
			writel(0x100203A0, 0x84);//TX  H232
			writel(0x100203A4, 0x84);//RX  H233
		}
		else if(index == 1)
		{
			//writel(0x10020020, 0x84);//TX  A008
			writel(0x10020024, 0x84);//RX  A009
		}
		fd_uart[index] = open(dev_uart[index], O_RDWR | O_NONBLOCK);
		if (fd_uart[index] < 0)
		{
			printf("open uart dev\n");
			return -1;
		}
		struct termios ios;
		tcgetattr(fd_uart[index],&ios);
		ios.c_lflag = 0;
		tcsetattr(fd_uart[index],TCSANOW,&ios);
	}

	return 0;
}

int UartGetFd(int index)
{
	return fd_uart[index];
}

int UartClose(int index)
{	
	int ret = -1;

	if(index < 0 || index > 2)
	{
		printf("no uart dev %d\n", index);
		return -1;
	}

	if(fd_uart[index] > 0)
	{
		ret = close(fd_uart[index]);
		fd_uart[index] = -1;		
	}

	return ret;
}

int UartSetAttribute(int index, COMM_ATTR *pattr)
{
	if(index < 0 || index > 2)
	{
		printf("no uart dev %d\n", index);
		return -1;
	}

	if (fd_uart[index] < 0)
	{
		printf("[%s:%d]not open uart dev\n", __FILE__, __LINE__);
		return -1;
	}		
	return SetAttribute(fd_uart[index], pattr);
}

int UartRead(int index, void *pdata, unsigned int nbytes)
{
	int err = 0;
	if(index < 0 || index > 2)
	{
		printf("no uart dev %d\n", index);
		return -1;
	}

	if (fd_uart[index] < 0)
	{
		printf("[%s:%d]not open uart dev\n", __FILE__, __LINE__);
		return -1;
	}
	err = read(fd_uart[index], pdata, nbytes);
	tcflush(fd_uart[index],TCIFLUSH);

	return err;
	
}

int UartWrite(int index, void *pdata, unsigned int len)
{
	if(index < 0 || index > 2)
	{
		printf("no uart dev %d\n", index);
		return -1;
	}
	
	if (fd_uart[index] < 0)
	{
		printf("[%s:%d]not open uart dev\n", __FILE__, __LINE__);
		return -1;
	}		
	return write(fd_uart[index], pdata, len);
}


// init uart_1
int UartInit(int id)
{
	int ret;

	ret = UartOpen(id);
	if (ret != 0)
	{
		printf("filed to open uart_%d\n", id);
		return -1;
	}

	COMM_ATTR attr;
	memset(&attr, 0, sizeof(attr));
	attr.baudrate = 9600;
	attr.databits = 8;
	attr.parity = 0;
	attr.stopbits = 1;
	ret = UartSetAttribute(id, &attr);
	if (ret != 0)
	{
		printf("filed to set uart_%d attribute.\n", id);
		return -1;
	}

	return 0;
}


int Uart1RecvData(unsigned char* pData, unsigned int maxlen)
{
	return UartRead(1, pData, maxlen);
}

int Uart1SendData(unsigned char* pData, unsigned int datalen)
{
	return UartWrite(1, pData, datalen);
}



